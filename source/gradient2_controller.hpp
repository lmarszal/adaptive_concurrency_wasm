#pragma once

#include <atomic>
#include <algorithm>
#include <stdint.h>
#include <mutex>

#include "percentile_samplewindow.hpp"
#include "expavg_measurement.hpp"
#include "gradient2_calculator.hpp"
#include "gradient.hpp"

class Gradient2Controller
{
    public:
        Gradient2Controller(uint32_t sample_window_size, double sample_window_percentile, uint32_t sample_window_time_ms)
        {
            // configure sample_window
            sample_window_ = new PercentileSampleWindow(sample_window_size, sample_window_percentile);
            this->sample_window_size = sample_window_size;
            this->sample_window_percentile = sample_window_percentile;

            // configure controller
            this->sample_window_time_ms = sample_window_time_ms;
        }

        void sample(double rtt, uint32_t inflight);
        Gradient tick(uint64_t now_ns, uint32_t limit);

    private:
        /* PercentileSampleWindow configuration */
        uint32_t sample_window_size;
        double sample_window_percentile = 0.9;
        std::atomic<PercentileSampleWindow*> sample_window_;

        /* ExpAvgMeasurement configuration */
        ExpAvgMeasurement measurement_;

        /* Controller configuration */
        uint32_t sample_window_time_ms;

        //absl::Mutex limit_mutation_mtx_;
        std::mutex limit_mutation_mtx_;
        std::atomic<uint64_t> next_update_time_ = 0;
        Gradient2Calculator calculator_;
        std::atomic<Gradient> gradient_ = EmptyGradient();
};
