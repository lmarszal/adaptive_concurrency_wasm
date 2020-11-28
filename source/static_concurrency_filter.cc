#include <atomic>

#include "proxy_wasm_intrinsics.h"
#include "proxy_wasm_common.h"
#include "nlohmann_json.hpp"

using json = nlohmann::json;

class PluginRootContext: public RootContext {
public:
  explicit PluginRootContext(uint32_t id, StringView root_id): RootContext(id, root_id) {
    proxy_set_tick_period_milliseconds(1000);
  }

  void onTick() override {
    inflight.record(num_rq_outstanding_.load());

    inflight.resolve();
    throttled.resolve();
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

    // TODO limit should be atomic
    limit_ = j["limit"].get<uint32_t>();

    return true;
  }

  uint32_t limit_;
  std::atomic<uint32_t> num_rq_outstanding_;
  Counter<> throttled = Counter("http_static_concurrency_filter_requests_throttled");

private:
  Gauge<> inflight = Gauge("http_static_concurrency_filter_requests_inflight");
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
};

// first event of the request
FilterHeadersStatus PluginContext::onRequestHeaders(uint32_t) {
  uint32_t current = ++root__->num_rq_outstanding_;
  if (current >= root__->limit_) {
    sendLocalResponse(503, "reached concurrency limit", nullptr, HeaderStringPairs());
    root__->throttled.increment(1);
    return FilterHeadersStatus::StopIteration;
  }
  return FilterHeadersStatus::Continue;
}

// last event of the request
void PluginContext::onLog() {
  --root__->num_rq_outstanding_;
}

static RegisterContextFactory register_PluginContext(CONTEXT_FACTORY(PluginContext), ROOT_FACTORY(PluginRootContext), "lmarszal.http.static_concurrency_filter");
