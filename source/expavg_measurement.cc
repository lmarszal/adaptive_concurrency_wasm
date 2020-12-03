#include "expavg_measurement.hpp"
#include <stdint.h>

double ExpAvgMeasurement::add(double sample)
{
    SharedDataSetResult res;
    ExpAvgMeasurementState state;
    do
    {
        uint32_t cas;
        state = state_.load(&cas);

        if (state.count < warmupWindow)
        {
            state.count++;
            state.sum += sample;
            state.value = state.sum/state.count;
        }
        else
        {
            double factor = 2.0 / (window + 1);
            state.value = state.value * (1.0 - factor) + sample * factor;
        }

        res = state_.set(state, cas);
    } while (res == SharedDataSetResult::CasMismatch);

    return state.value;
}

void ExpAvgMeasurement::set(double value)
{
    SharedDataSetResult res;
    ExpAvgMeasurementState state;
    do
    {
        uint32_t cas;
        auto state = state_.load(&cas);
        state.value = value;

        res = state_.set(state, cas);
    } while (res == SharedDataSetResult::CasMismatch);
}

double ExpAvgMeasurement::get()
{
    uint32_t _cas;
    return state_.load(&_cas).value;
}
