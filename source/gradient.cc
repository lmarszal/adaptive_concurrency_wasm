#include "gradient.hpp"

Gradient EmptyGradient()
{
    return (struct Gradient){.limit = 0, .shortRtt = 0, .longRtt = 0, .inflight = 0};
}
