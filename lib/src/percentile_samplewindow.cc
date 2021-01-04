#include "percentile_samplewindow.hpp"

#include <algorithm>
#include <cmath>

void PercentileSampleWindow::add(double rtt, uint32_t inflight)
{
    if (i_++ < window)
    {
        samples_[i_] = rtt;
    }
    if (max_inflight_ < inflight)
    {
        max_inflight_ = inflight;
    }
}

double PercentileSampleWindow::get()
{
    std::sort(samples_, samples_+window);
    auto k = std::clamp((uint32_t)std::ceil(window*percentile), (uint32_t)0, window-1);
    return samples_[k];
}
