#include "gradient_metrics.hpp"

void GradientMetrics::resolve(Gradient gradient)
{
    if(!started_.load())
    {
        return;
    }

    // gradient metrics
    limit.record(gradient.limit);
    shortRtt.record(gradient.shortRtt);
    longRtt.record(gradient.longRtt);
    maxInflight.record(gradient.inflight);

    limit.resolve();
    shortRtt.resolve();
    longRtt.resolve();
    maxInflight.resolve();

    // throttled metric
    throttled.resolve();
}

void GradientMetrics::incThrottled()
{
    throttled.increment(1);
}