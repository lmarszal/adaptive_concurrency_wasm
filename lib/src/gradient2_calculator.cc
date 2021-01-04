#include "gradient2_calculator.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

uint32_t smoothLimit(uint32_t oldLmit, double newLimit)
{
    const double smoothing = 0.2;
    auto limit = (uint32_t)std::floor((double)oldLmit * (1.0 - smoothing) + newLimit * smoothing);
    if (limit != oldLmit)
    {
        return limit;
    }

    // smoothing shouldn't cause limit to be constant
    if (newLimit > oldLmit)
    {
        return oldLmit+1;
    }
    else 
    {
        return oldLmit-1;
    }
    
}

uint32_t Gradient2Calculator::calculateLimit(double shortRtt, double longRtt, uint32_t limit, uint32_t inflight)
{
    // allow some queueing
    auto queueSize = std::max(std::sqrt(limit), 4.0);

    // Slightly shrink the limit if we are app limited
    if (inflight < limit / 2) {
        return std::floor(0.95 * limit);
    }

    // Rtt could be higher than rtt_noload because of smoothing rtt noload updates
    // so set to 1.0 to indicate no queuing.  Otherwise calculate the slope and don't
    // allow it to be reduced by more than half to avoid aggressive load-shedding due to 
    // outliers.
    auto gradient = std::clamp((tolerance * longRtt) / shortRtt, 0.5, 1.0);
    auto newLimit = gradient * limit;
    // allow some queueing if system is stable
    if (gradient > 0.75)
    {
        newLimit += queueSize;
    }
    uint32_t newLimit_ui32 = smoothLimit(limit, newLimit);

    return std::clamp(newLimit_ui32, min_limit, max_limit);
}
