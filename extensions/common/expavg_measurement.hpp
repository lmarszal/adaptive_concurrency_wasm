#pragma once

#include <atomic>
#include "proxy_wasm_intrinsics.h"

class ExpAvgMeasurement
{
    public:
        double add(uint32_t sample)
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

        void set(double value)
        {
            value_ = value;
        }

        double get()
        {
            return value_;
        }

    private:
        double value_ = 0.0;
        double sum_ = 0.0;
        uint32_t window = 600;
        uint32_t warmupWindow = 10;
        uint32_t count_ = 0;
};


