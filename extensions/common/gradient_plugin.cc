#include "gradient_plugin.hpp"

#include "proxy_wasm_common.h"
#include "nlohmann_json.hpp"

using json = nlohmann::json;


bool GradientPluginRootContext::onConfigure(size_t configuration_size) {
    auto cfg = getConfiguration()->toString();

    if (!json::accept(cfg)) {
        LOG_ERROR("invalid configuration json");
        return false;
    }

    auto j = json::parse(cfg, nullptr, false);

    if (!j.contains("limit") || !j["limit"].is_number_integer()) {
        LOG_ERROR("limit (integer) is required");
        return false;
    }
    auto limit = j["limit"].get<uint32_t>();
    initial_limit_ = limit;
    uint32_t zero;
    limit_.compare_exchange_strong(zero, limit);

    uint32_t window_size = 5000;
    if (j.contains("window_size_ms")) 
    {
        if (!j["window_size_ms"].is_number_integer()) {
        LOG_ERROR("window_size_ms must be integer");
        return false;
        }
        window_size = j["window_size_ms"].get<uint32_t>();
    }

    auto old = ctrl_.exchange(new Gradient2Controller(50, 0.9, window_size));
    if (old != nullptr)
    {
        delete old;
    }

    return true;
}

void GradientPluginRootContext::onTick() {
    auto now = getCurrentTimeNanoseconds();
    auto limit = limit_.load();
    auto ctrl = ctrl_.load();
    if (ctrl != nullptr)
    {
        auto gradient = ctrl->tick(now, limit);
        limit_.exchange(gradient.limit);
        metrics_.resolve(gradient);
    }
    else
    {
        metrics_.resolve(EmptyGradient());
    }
}

void GradientPluginRootContext::reportThrottled()
{
    metrics_.incThrottled();
}

void GradientPluginRootContext::sample(double rtt, uint32_t inflight)
{
    auto ctrl = ctrl_.load();
    if (ctrl == nullptr) {
      return;
    }
    ctrl->sample(rtt, inflight);
}

// first event of the request
FilterHeadersStatus GradientPluginContext::onRequestHeaders(uint32_t) {
  uint32_t current = root__->num_rq_outstanding_.fetch_add(1);
  inflight = -1;
  auto limit = root__->getLimit();
  if (current >= limit && limit > 0) {
    sendLocalResponse(503, "reached concurrency limit", nullptr, HeaderStringPairs());
    root__->reportThrottled();
    return FilterHeadersStatus::StopIteration;
  }
  inflight = current + 1;
  start = getCurrentTimeNanoseconds();
  return FilterHeadersStatus::Continue;
}

// last event of the request
void GradientPluginContext::onLog() {
  auto now = getCurrentTimeNanoseconds();
  root__->num_rq_outstanding_.fetch_sub(1);

  // act only on not-throttled requests
  if (inflight > 0)
  { 
    auto rtt = ((double)now - start) * 1e-6;
    root__->sample(rtt, inflight);
  }
}
