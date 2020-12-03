#pragma once

#include "proxy_wasm_intrinsics.h"
#include "gradient.hpp"
#include <atomic>

class GradientMetrics
{
public:
    // TODO label metrics with listener name
    void resolve(Gradient gradient);
    void incThrottled();
    void startReporting()
    {
        started_ = true;
    }
private:
    Gauge<> limit = Gauge<>("http_adaptive_concurrency_filter_requests_limit");
    Gauge<> shortRtt = Gauge<>("http_adaptive_concurrency_filter_requests_short_rtt");
    Gauge<> longRtt = Gauge<>("http_adaptive_concurrency_filter_requests_long_rtt");
    Gauge<> maxInflight = Gauge<>("http_adaptive_concurrency_filter_requests_max_inflight");
    Counter<> throttled = Counter<>("http_adaptive_concurrency_filter_requests_throttled");
    std::atomic<bool> started_ = false;
};
