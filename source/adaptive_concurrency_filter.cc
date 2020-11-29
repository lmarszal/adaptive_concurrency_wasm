#include <atomic>

#include "proxy_wasm_intrinsics.h"
#include "proxy_wasm_common.h"
#include "nlohmann_json.hpp"

#include "common/gradient2_controller.hpp"

using json = nlohmann::json;

class PluginRootContext: public RootContext {
public:
  explicit PluginRootContext(uint32_t id, StringView root_id): RootContext(id, root_id) {
    proxy_set_tick_period_milliseconds(1000);
  }

  void onTick() override {
    throttled.resolve();
    limit.resolve();
    shortRtt.resolve();
    longRtt.resolve();
    maxInflight.resolve();
  }

  bool onConfigure(size_t configuration_size) override {
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

    limit_ = j["limit"].get<uint32_t>();

    return true;
  }

  std::atomic<uint32_t> limit_;
  std::atomic<uint32_t> num_rq_outstanding_;
  Counter<> throttled = Counter("http_adaptive_concurrency_filter_requests_throttled");

  // TODO support config reloading
  Gradient2Controller ctrl = Gradient2Controller(50, 0.9, 60000);
  Gauge<> limit = Gauge("http_adaptive_concurrency_filter_requests_limit");
  Gauge<> shortRtt = Gauge("http_adaptive_concurrency_filter_requests_short_rtt");
  Gauge<> longRtt = Gauge("http_adaptive_concurrency_filter_requests_long_rtt");
  Gauge<> maxInflight = Gauge("http_adaptive_concurrency_filter_requests_max_inflight");
};

class PluginContext : public Context {
public:
  explicit PluginContext(uint32_t id, RootContext* root) : Context(id, root) {
    this->root__ = (PluginRootContext*)root;
  }

  FilterHeadersStatus onRequestHeaders(uint32_t) override;
  void onLog() override;

private:
  PluginRootContext* root__;
  uint64_t start;
  uint32_t inflight;
};

// first event of the request
FilterHeadersStatus PluginContext::onRequestHeaders(uint32_t) {
  uint32_t current = root__->num_rq_outstanding_.fetch_add(1);
  if (current >= root__->limit_.load()) {
    sendLocalResponse(503, "reached concurrency limit", nullptr, HeaderStringPairs());
    root__->throttled.increment(1);
    return FilterHeadersStatus::StopIteration;
  }
  start = getCurrentTimeNanoseconds();
  inflight = current;
  return FilterHeadersStatus::Continue;
}

// last event of the request
void PluginContext::onLog() {
  --root__->num_rq_outstanding_;
  auto now = getCurrentTimeNanoseconds();
  auto rtt = (uint32_t)((now - start) / 1000);
  
  auto gradient = root__->ctrl.sample(now, rtt, root__->limit_.load(), inflight);
  root__->limit.record(gradient.limit);
  root__->shortRtt.record(gradient.shortRtt);
  root__->longRtt.record(gradient.longRtt);
  root__->maxInflight.record(gradient.inflight);
  root__->limit_.exchange(gradient.limit);
}

static RegisterContextFactory register_PluginContext(CONTEXT_FACTORY(PluginContext), ROOT_FACTORY(PluginRootContext), "lmarszal.http.adaptive_concurrency_filter");
