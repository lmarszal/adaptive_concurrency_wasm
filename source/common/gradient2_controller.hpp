#pragma once

#include <atomic>
#include <algorithm>
#include "proxy_wasm_intrinsics.h"

#include "percentile_samplewindow.hpp"
#include "expavg_measurement.hpp"
#include "gradient2_calculator.hpp"
//#include "absl/synchronization/mutex.h"
#include <mutex>

struct Gradient
{
    uint32_t limit;
    double shortRtt;
    double longRtt;
    uint32_t inflight;
};

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

        Gradient sample(uint64_t now_ns, double rtt, uint32_t limit, uint32_t inflight)
        {
            auto sample_window = sample_window_.load();
            sample_window->add(rtt, inflight);

            if (now_ns > next_update_time_.load())
            {
                //absl::MutexLock ml(&limit_mutation_mtx_);
                std::lock_guard<std::mutex> guard(limit_mutation_mtx_);
                // Double check under the lock
                if (now_ns > next_update_time_.load())
                {
                    auto sample_window = sample_window_.exchange(new PercentileSampleWindow(sample_window_size, sample_window_percentile));
                    auto shortRtt = sample_window->get();
                    auto longRtt = measurement_.add(shortRtt);
                    auto maxInflight = sample_window->maxInflight();

                    uint64_t next_update_time = now_ns + ((uint64_t)sample_window_time_ms * 1e+6);
                    next_update_time_.exchange(next_update_time);

                    if (sample_window->ready())
                    {
                        // If the long RTT is substantially larger than the short RTT then reduce the long RTT measurement.
                        // This can happen when latency returns to normal after a prolonged prior of excessive load.  Reducing the
                        // long RTT without waiting for the exponential smoothing helps bring the system back to steady state.
                        if (longRtt / shortRtt > 2.0) {
                            measurement_.set(longRtt * 0.95);
                        }

                        limit = calculator_.calculateLimit(shortRtt, longRtt, limit, maxInflight);
                    }

                    delete sample_window;

                    // TODO report #updates

                    gradient_ = (struct Gradient){.limit = limit, .shortRtt = shortRtt, .longRtt = longRtt, .inflight = maxInflight};

                    char buffer[1024];
                    sprintf(buffer, "now=%llu, next_update_time=%llu, limit=%u, shortRtt=%f, longRtt=%f, inflight=%u", now_ns, next_update_time, limit, shortRtt, longRtt, maxInflight);
                    LOG_WARN(buffer);
                }
            }

            return gradient_.load();
        }

    private:
        /* PercentileSampleWindow configuration */
        uint32_t sample_window_size;
        double sample_window_percentile;
        std::atomic<PercentileSampleWindow*> sample_window_;

        /* ExpAvgMeasurement configuration */
        ExpAvgMeasurement measurement_;

        /* Controller configuration */
        uint32_t sample_window_time_ms;

        //absl::Mutex limit_mutation_mtx_;
        std::mutex limit_mutation_mtx_;
        std::atomic<uint64_t> next_update_time_ = 0;
        Gradient2Calculator calculator_;
        std::atomic<Gradient> gradient_ = (struct Gradient){.limit = 3, .shortRtt = 0, .longRtt = 0, .inflight = 0};
};
