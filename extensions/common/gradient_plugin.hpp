#pragma once

#include <atomic>
#include <string>
#include "proxy_wasm_intrinsics.h"

#include "gradient2_controller.hpp"
#include "gradient_metrics.hpp"
#include "shared_data.hpp"

class GradientPluginRootContext: public RootContext {
public:
  explicit GradientPluginRootContext(uint32_t id, StringView root_id): RootContext(id, root_id) {
    proxy_set_tick_period_milliseconds(100);
  }

  bool onConfigure(size_t configuration_size) override;
  void onTick() override;

  SharedInt num_rq_outstanding_ = SharedInt("lmarszal_adaptive_concurrency_num_rq_outstanding_");
  virtual uint32_t getLimit() = 0;
  void reportThrottled();
  void startReporting();
  void sample(double rtt, uint32_t inflight);

  std::string cluster_name_;

protected:
  std::atomic<uint32_t> initial_limit_ = 0;
  SharedInt limit_ = SharedInt("lmarszal_adaptive_concurrency_limit_");

private:
  std::atomic<Gradient2Controller*> ctrl_ = nullptr;
  GradientMetrics metrics_;
};

class GradientPluginContext : public Context {
public:
  explicit GradientPluginContext(uint32_t id, RootContext* root) : Context(id, root) {
    this->root__ = (GradientPluginRootContext*)root;
  }

  FilterHeadersStatus onRequestHeaders(uint32_t) override;
  void onLog() override;

private:
  GradientPluginRootContext* root__;
  uint64_t start;
  int32_t inflight;
  bool unwanted_cluster_ = false;
};
