#include "gradient2_calculator.hpp"

#include <algorithm>
#include <cmath>

uint32_t Gradient2Calculator::calculateLimit(double shortRtt, double longRtt, uint32_t limit, uint32_t inflight)
{
    // allow some queueing
    auto queueSize = std::max(std::sqrt(limit), 4.0);

    // Don't grow the limit if we are app limited
    if (inflight < limit / 2) {
        return limit;
    }

    // Rtt could be higher than rtt_noload because of smoothing rtt noload updates
    // so set to 1.0 to indicate no queuing.  Otherwise calculate the slope and don't
    // allow it to be reduced by more than half to avoid aggressive load-shedding due to 
    // outliers.
    auto gradient = std::clamp(tolerance * longRtt / shortRtt, 0.5, 1.0);
    auto newLimit = gradient * limit + queueSize;
    newLimit = limit * (1 - smoothing) + newLimit * smoothing;
    uint32_t newLimit_ui32 = std::clamp((uint32_t)newLimit, min_limit, max_limit);

    return newLimit_ui32;
}
