#include "percentile_samplewindow.hpp"

#include <algorithm>
#include <cmath>

void PercentileSampleWindow::add(double rtt, uint32_t inflight)
{
    uint32_t i = i_.fetch_add(1);
    if (i < window)
    {
        samples_[i] = rtt;
    }
    while(max_inflight_.load() < inflight)
    {
        max_inflight_.exchange(inflight);
    }
}

double PercentileSampleWindow::get()
{
    std::sort(samples_, samples_+window);
    auto k = std::clamp((uint32_t)std::ceil(window*percentile), (uint32_t)0, window-1);
    return samples_[k];
}
