#include "gradient2_controller.hpp"

void Gradient2Controller::sample(double rtt, uint32_t inflight)
{
    sample_window_.add(rtt, inflight);
}

Gradient Gradient2Controller::update(uint64_t now_ns, uint32_t limit)
{
    gradient_.limit = limit; // assing in case gradient object is empty

    if (now_ns > next_update_time_)
    {
        next_update_time_ = now_ns + sample_window_time_ns;

        double shortRtt = 0;
        double longRtt = measurement_.get();
        auto maxInflight = sample_window_.maxInflight();

        if (sample_window_.ready())
        {
            shortRtt = sample_window_.get();
            longRtt = measurement_.add(shortRtt);
        
            // If the long RTT is substantially larger than the short RTT then reduce the long RTT measurement.
            // This can happen when latency returns to normal after a prolonged prior of excessive load.  Reducing the
            // long RTT without waiting for the exponential smoothing helps bring the system back to steady state.
            if (longRtt / shortRtt > 2.0) {
                measurement_.set(longRtt * 0.95);
            } else if ((longRtt / shortRtt > 1.4) && (limit / maxInflight > 2)) {
                measurement_.set(longRtt * 0.95);
            }

            limit = calculator_.calculateLimit(shortRtt, longRtt, limit, maxInflight);

        }

        gradient_ = (struct Gradient){.limit = limit, .shortRtt = shortRtt, .longRtt = longRtt, .inflight = maxInflight};

        sample_window_.reset();
    }

    return gradient_;
}
