#pragma once
#include "proxy_wasm_intrinsics.h"
#include <cstdlib>
#include "rtt_queue.hpp"
#include "leader.hpp"
#include "gradient2_controller.hpp"
#include "config.hpp"

const uint32_t tick_period_ms = 100;
const uint64_t tick_period_ns = (uint64_t)tick_period_ms * 1e6;

class PluginRootContext: public RootContext {
public:
    explicit PluginRootContext(uint32_t id, StringView root_id): RootContext(id, root_id) {
        proxy_set_tick_period_milliseconds(tick_period_ms);
    }

    bool onStart(size_t /* vm_configuration_size */) override;
    void onTick() override;
    bool onConfigure(size_t configuration_size) override;
    virtual uint32_t getLimitInternal();

    RttQueue rtt_queue_;
private:
    Gradient2Controller ctrl = Gradient2Controller(50, 0.9, config_default_window_size);
    LeaderTokenLease lease_;

    void onLeaderStart(uint64_t now_ns);
    void onLeaderTick(uint64_t now_ns);
protected:
  Config config_ = default_config();
};

class PluginContext : public Context {
public:
  explicit PluginContext(uint32_t id, RootContext* root) : Context(id, root) {
    this->root__ = (PluginRootContext*)root;
  }

  FilterHeadersStatus onRequestHeaders(uint32_t) override;
  void onLog() override;

  void rttEnqueue(RttData data)
  {
      root__->rtt_queue_.Enqueue(data);
  }
private:
  PluginRootContext* root__;
  uint64_t start_ = 0;
  uint32_t inflight_ = 0;
};
