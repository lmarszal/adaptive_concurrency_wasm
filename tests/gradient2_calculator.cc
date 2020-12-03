#include "gtest/gtest.h"
#include "gradient2_calculator.hpp"
#include <algorithm>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}

TEST(Gradient2Calculator, should_grow_limit_when_shortRTT_is_smaller_than_longRTT)
{
    Gradient2Calculator sut;
    double shortRtt = 30;
    double longRtt = 100;
    uint32_t limit = 30;
    uint32_t inflight = 20;

    auto actual = sut.calculateLimit(shortRtt, longRtt, limit, inflight);
    EXPECT_GT(actual, limit);
}
TEST(Gradient2Calculator, should_grow_minimal_limit)
{
    Gradient2Calculator sut;
    double shortRtt = 30;
    double longRtt = 100;
    uint32_t limit = 3;
    uint32_t inflight = 3;

    auto actual = sut.calculateLimit(shortRtt, longRtt, limit, inflight);
    EXPECT_GT(actual, limit);
}

TEST(Gradient2Calculator, should_shrink_limit_when_shortRTT_is_bigger_than_longRTT)
{
    Gradient2Calculator sut;
    double shortRtt = 100;
    double longRtt = 30;
    uint32_t limit = 30;
    uint32_t inflight = 20;

    auto actual = sut.calculateLimit(shortRtt, longRtt, limit, inflight);
    EXPECT_LT(actual, limit);
}

TEST(Gradient2Calculator, should_stabilize_when_short_and_long_rtt_are_close)
{
    Gradient2Calculator sut;
    uint32_t limit = 100;
    uint32_t initial_limit = 100;

    for (int i=0; i<1000; i++)
    {
        auto actual = sut.calculateLimit(60, 30, limit, limit);
        EXPECT_GT(1.0 * actual, 0.8 * initial_limit) << "i=" << i;
        EXPECT_LT(1.0 * actual, 1.2 * initial_limit) << "i=" << i;
        limit = actual;

        actual = sut.calculateLimit(45, 30, limit, limit);
        EXPECT_GT(1.0 * actual, 0.8 * initial_limit) << "i=" << i;
        EXPECT_LT(1.0 * actual, 1.2 * initial_limit) << "i=" << i;
        limit = actual;
    }
}

TEST(Gradient2Calculator, dont_grow_the_limit_when_were_app_limited)
{
    Gradient2Calculator sut;
    double shortRtt = 30;
    double longRtt = 100;
    uint32_t limit = 100;
    uint32_t inflight = 2;

    auto actual = sut.calculateLimit(shortRtt, longRtt, limit, inflight);
    EXPECT_EQ(actual, limit);
}
