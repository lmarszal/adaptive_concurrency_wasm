#pragma once

#include <atomic>
#include <algorithm>
#include "proxy_wasm_intrinsics.h"

class PercentileSampleWindow
{
    public:
        PercentileSampleWindow(uint32_t window, double percentile)
        {
            this->window = window;
            this->percentile = percentile;
            this->samples_ = new uint64_t[window]; // TODO figure out how to reuse this buffer
        }

        ~PercentileSampleWindow()
        {
            delete this->samples_;
        }

        void add(uint64_t rtt, uint32_t inflight)
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

        /*
         * Not get is not thread safe - you should exchange reference to sample window first and only then use get()
         */
        uint64_t get()
        {
            std::sort(samples_, samples_+window);
            auto k = std::clamp((uint32_t)std::ceil(window*percentile), (uint32_t)0, window-1);
            return samples_[k];
        }

        uint32_t maxInflight()
        {
            return max_inflight_.load();
        }

        bool ready()
        {
            return i_.load() > window;
        }

    private:
        double percentile;
        uint32_t window;
        std::atomic<uint32_t> i_ = 0;
        std::atomic<uint32_t> max_inflight_ = 0;
        uint64_t* samples_;
};
