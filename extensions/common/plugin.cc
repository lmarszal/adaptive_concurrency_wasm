#include "plugin.hpp"
#include "rtt_queue.hpp"
#include "state.hpp"
#include "expavg_measurement.hpp"

const uint32_t initial_limit = 5;

Gauge<> LIMIT_METRIC = Gauge<>("http_adaptive_concurrency_filter_requests_limit");
Gauge<> SHORT_RTT_METRIC = Gauge<>("http_adaptive_concurrency_filter_requests_short_rtt");
Gauge<> LONG_RTT_METRIC = Gauge<>("http_adaptive_concurrency_filter_requests_long_rtt");
Gauge<> MAX_INFLIGHT_METRIC = Gauge<>("http_adaptive_concurrency_filter_requests_max_inflight");
Counter<> THROTTLED_METRIC = Counter<>("http_adaptive_concurrency_filter_requests_throttled");

bool PluginRootContext::onStart(size_t /* vm_configuration_size */)
{
    // ensure queue is created
    rtt_queue_.Ensure(config_.vm_name);

    // ensure state is initialized
    ensureLimit(5);
    ensureInflight();

    return true;
}

void PluginRootContext::onTick()
{
    auto now_ns = getCurrentTimeNanoseconds();

    if (lease_.isExpired(now_ns))
    {
        auto success = lease_.tryAckquire(now_ns, 2 * tick_period_ns);
        if (success)
        {
            onLeaderStart(now_ns);
        }
    }
    
    if (lease_.isMine())
    {
        lease_.extend(now_ns, 2 * tick_period_ns);
        onLeaderTick(now_ns);   
    }
}

void PluginRootContext::onLeaderStart(uint64_t now_ns)
{
    LOG_WARN("leader reelection");
    ctrl.reset(now_ns);
}

void PluginRootContext::onLeaderTick(uint64_t now_ns)
{
    RttData elem;
    while (rtt_queue_.Dequeue(elem))
    {
        ctrl.sample(elem.rtt(), elem.inflight());
    }

    auto expAvgState = getExpAvgState();
    ctrl.restoreMeasurement(expAvgState);
    auto limit = getLimit();
    auto gradient = ctrl.update(now_ns, limit);
    setExpAvgState(ctrl.storeMeasurement());
    setLimit(gradient.limit);
    
    LIMIT_METRIC.record(gradient.limit);
    LONG_RTT_METRIC.record(gradient.longRtt);
    SHORT_RTT_METRIC.record(gradient.shortRtt);
    MAX_INFLIGHT_METRIC.record(gradient.inflight);

    LIMIT_METRIC.resolve();
    LONG_RTT_METRIC.resolve();
    SHORT_RTT_METRIC.resolve();
    MAX_INFLIGHT_METRIC.resolve();
    THROTTLED_METRIC.resolve();
}

bool PluginRootContext::onConfigure(size_t configuration_size)
{
    config_ = load_config();
    rtt_queue_.Ensure(config_.vm_name);
    return true;
}

uint32_t PluginRootContext::getLimit()
{
    return getLimit();
}

FilterHeadersStatus PluginContext::onRequestHeaders(uint32_t)
{
    start_ = getCurrentTimeNanoseconds();
    auto current = incrementInflight();
    auto limit = root__->getLimit();
    if (current > limit && limit > 0)
    {
        sendLocalResponse(503, "reached concurrency limit", nullptr, HeaderStringPairs());
        THROTTLED_METRIC.increment(1);
        return FilterHeadersStatus::StopIteration;
    }
    inflight_ = current;
    return FilterHeadersStatus::Continue;
}

void PluginContext::onLog()
{
    decrementInflight();

    // don't sample throttled requests
    if (inflight_ > 0)
    {
        auto now = getCurrentTimeNanoseconds();
        double rtt = ((double)now - start_) * 1e-6;
        rttEnqueue(RttData(rtt, inflight_));
    }
}
