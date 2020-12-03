#include "gradient2_controller.hpp"

void Gradient2Controller::sample(double rtt, uint32_t inflight)
{
    auto sample_window = sample_window_.load();
    sample_window->add(rtt, inflight);
}

Gradient Gradient2Controller::tick(uint64_t now_ns, uint32_t limit)
{
    std::lock_guard<std::mutex> guard(limit_mutation_mtx_);
    // TODO synchronize-exchange next_update_time
    if (now_ns > next_update_time_.load())
    {
        auto sample_window = sample_window_.exchange(new PercentileSampleWindow(sample_window_size, sample_window_percentile));

        uint64_t next_update_time = now_ns + ((uint64_t)sample_window_time_ms * 1e+6);
        next_update_time_.exchange(next_update_time);

        double shortRtt = 0;
        double longRtt = measurement_.get();
        auto maxInflight = sample_window->maxInflight();

        if (sample_window->ready())
        {
            shortRtt = sample_window->get();
            longRtt = measurement_.add(shortRtt);
        
            // If the long RTT is substantially larger than the short RTT then reduce the long RTT measurement.
            // This can happen when latency returns to normal after a prolonged prior of excessive load.  Reducing the
            // long RTT without waiting for the exponential smoothing helps bring the system back to steady state.
            if (longRtt / shortRtt > 2.0) {
                measurement_.set(longRtt * 0.95);
            }

            limit = calculator_.calculateLimit(shortRtt, longRtt, limit, maxInflight);

        }

        gradient_ = (struct Gradient){.limit = limit, .shortRtt = shortRtt, .longRtt = longRtt, .inflight = maxInflight};

        // TODO risk of race condition
        delete sample_window;
    }

    return gradient_.load();
}
