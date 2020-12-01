#pragma once

#include <atomic>
#include <stdint.h>

class PercentileSampleWindow
{
    public:
        PercentileSampleWindow(uint32_t window, double percentile)
        {
            this->window = window;
            this->percentile = percentile;
            this->samples_ = new double[window]; // TODO figure out how to reuse this buffer
        }

        ~PercentileSampleWindow()
        {
            delete this->samples_;
        }

        void add(double rtt, uint32_t inflight);

        /*
         * Note get is not thread safe - you should exchange reference to sample window first and only then use get()
         */
        double get();

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
        double* samples_;
};
