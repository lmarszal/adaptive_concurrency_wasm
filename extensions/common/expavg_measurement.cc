#include "expavg_measurement.hpp"
#include <stdint.h>

double ExpAvgMeasurement::add(uint32_t sample)
{
    if (count_ < warmupWindow)
    {
        count_++;
        sum_ += sample;
        value_ = sum_/count_;
    }
    else
    {
        double factor = 2.0 / (window + 1);
        value_ = value_ * (1.0 - factor) + sample * factor;
    }
    return value_;
}

void ExpAvgMeasurement::set(double value)
{
    value_ = value;
}

double ExpAvgMeasurement::get()
{
    return value_;
}
