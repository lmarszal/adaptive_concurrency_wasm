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
        Gradient2Controller(uint32_t sample_window_size, double sample_window_percentile, uint64_t sample_window_time_ms) :
            sample_window_(sample_window_size, sample_window_percentile)
        {
            // configure controller
            this->sample_window_time_ns = sample_window_time_ms * 1e6;
        }
        
        uint64_t windowTime()
        {
            return sample_window_time_ns;
        }
        bool ready(uint64_t now_ns)
        {
            return now_ns > next_update_time_;
        }
        void restoreMeasurement(std::string state)
        {
            measurement_ = measurement_.fromString(state);
        }
        std::string storeMeasurement()
        {
            return measurement_.toString();
        }
        void reset(uint64_t now_ns)
        {
            next_update_time_ = now_ns + sample_window_time_ns;
            sample_window_.reset();
        }
        void setSampleWindowTime(uint64_t sample_window_time_ms)
        {
            sample_window_time_ns = sample_window_time_ms * 1e6;
        }
        void sample(double rtt, uint32_t inflight);
        Gradient update(uint64_t now_ns, uint32_t limit);

    private:
        /* PercentileSampleWindow configuration */
        PercentileSampleWindow sample_window_;

        /* ExpAvgMeasurement configuration */
        ExpAvgMeasurement measurement_;

        /* Controller configuration */
        uint64_t sample_window_time_ns;

        //absl::Mutex limit_mutation_mtx_;
        std::mutex limit_mutation_mtx_;
        uint64_t next_update_time_ = 0;
        Gradient2Calculator calculator_;
        Gradient gradient_ = EmptyGradient();
};
