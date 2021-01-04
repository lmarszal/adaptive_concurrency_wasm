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
            this->samples_ = new double[window];
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
            return max_inflight_;
        }

        bool ready()
        {
            return i_ > window;
        }

        void reset()
        {
            i_ = 0;
            max_inflight_ = 0;
        }

    private:
        double percentile;
        uint32_t window;
        uint32_t i_ = 0;
        uint32_t max_inflight_ = 0;
        double* samples_;
};
