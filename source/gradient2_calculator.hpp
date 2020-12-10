#pragma once

#include <stdint.h>

class Gradient2Calculator
{
    public:
        uint32_t calculateLimit(double shortRtt, double longRtt, uint32_t limit, uint32_t inflight);

    private:
        double tolerance = 1.5;
        uint32_t min_limit = 3;
        uint32_t max_limit = 2048;
};
