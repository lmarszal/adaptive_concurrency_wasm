#pragma once

#include <cstdint>
#include <string>
#include <cstdio>
#include "proxy_wasm_intrinsics.h"

class RttData
{
public:
    RttData(double rtt, uint32_t inflight)
    {
        rtt_ = rtt;
        inflight_ = inflight;
    }

    RttData() : RttData(0, 0) { }

    static RttData fromString(std::string str)
    {
        double rtt;
        uint32_t inflight;
        sscanf(str.c_str(), "%lf:%u", &rtt, &inflight);
        return RttData(rtt, inflight);
    }

    std::string toString()
    {
        return std::to_string(rtt_) + ":" + std::to_string(inflight_);
    }

    double rtt()
    {
        return rtt_;
    }

    uint32_t inflight()
    {
        return inflight_;
    }
private:
    double rtt_;
    uint32_t inflight_;
};

class RttQueue
{
public:
    RttQueue()
    {
        std::srand(getCurrentTimeNanoseconds());
        unique_id_ = std::rand();
    }

    bool Dequeue(RttData& elem);
    void Enqueue(RttData elem);
    void Ensure();
private:
    uint32_t rtt_queue_token_;
    uint32_t unique_id_;
};
