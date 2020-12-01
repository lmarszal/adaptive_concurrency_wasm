#pragma once

#include <stdint.h>

class ExpAvgMeasurement
{
    public:
        double add(uint32_t sample);
        void set(double value);
        double get();

    private:
        double value_ = 0.0;
        double sum_ = 0.0;
        uint32_t window = 600;
        uint32_t warmupWindow = 10;
        uint32_t count_ = 0;
};


