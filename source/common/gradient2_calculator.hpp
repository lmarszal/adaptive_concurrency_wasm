#pragma once

#include "proxy_wasm_intrinsics.h"

class Gradient2Calculator
{
    public:
        uint32_t calculateLimit(double shortRtt, double longRtt, uint32_t limit, uint32_t inflight)
        {
            // Don't grow the limit if we are app limited
            if (inflight < limit / 2) {
                return limit;
            }

            // Rtt could be higher than rtt_noload because of smoothing rtt noload updates
            // so set to 1.0 to indicate no queuing.  Otherwise calculate the slope and don't
            // allow it to be reduced by more than half to avoid aggressive load-shedding due to 
            // outliers.
            auto gradient = std::clamp(tolerance * longRtt / shortRtt, 0.5, 1.0);
            auto newLimit = limit * gradient + limit;
            newLimit = limit * (1 - smoothing) + newLimit * smoothing;
            uint32_t newLimit_ui32 = std::clamp((uint32_t)newLimit, min_limit, max_limit);

            return newLimit_ui32;
        }

    private:
        double tolerance = 1.5;
        double smoothing = 0.2;
        uint32_t min_limit = 3;
        uint32_t max_limit = 2048;
};
