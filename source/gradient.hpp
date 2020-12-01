#pragma once

#include <stdint.h>

struct Gradient
{
    uint32_t limit;
    double shortRtt;
    double longRtt;
    uint32_t inflight;
};

Gradient EmptyGradient();
