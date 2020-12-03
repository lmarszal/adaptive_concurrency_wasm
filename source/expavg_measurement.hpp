#pragma once

#include <stdint.h>
#include "shared_data.hpp"

class ExpAvgMeasurement
{
    public:
        double add(double sample);
        void set(double value);
        double get();

    private:
        SharedExpAvgMeasurementState state_ = SharedExpAvgMeasurementState("lmarszal_adaptive_concurrency_exp_avg_state");
        uint32_t window = 600;
        uint32_t warmupWindow = 10;
};


