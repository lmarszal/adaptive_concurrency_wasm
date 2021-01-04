#include "gtest/gtest.h"
#include "expavg_measurement.hpp"
#include <algorithm>

TEST(ExpAvgMeasurement, should_return_arithmetic_avg_during_warmup_period)
{
    ExpAvgMeasurement sut;
    for (int i=0; i<10; i++)
    {
        sut.add(1.0 + i);
    }
    double actual = sut.get();
    double expected = 5.5;
    EXPECT_EQ(expected, actual);
}

TEST(ExpAvgMeasurement, should_grow_and_shrink_as_you_add_values)
{
    ExpAvgMeasurement sut;
    for (int i=0; i<10; i++)
    {
        sut.add(1.0);
    }

    double given;
    
    given = 5.0;
    auto actual1 = sut.add(given);
    EXPECT_GT(actual1, 1.0) << "expected exp avg to grow as you add bigger value";
    EXPECT_LT(actual1, given) << "expected exp avg to be less than added value" ;
    given = 5.0;
    auto actual2 = sut.add(given);
    EXPECT_GT(actual2, actual1) << "expected exp avg to grow as you add bigger value";
    EXPECT_LT(actual2, given) << "expected exp avg to be less than added value" ;
    given = actual2/2;
    auto actual3 = sut.add(given);
    EXPECT_LT(actual3, actual2) << "expected exp avg to shrink as you add smaller value";
    EXPECT_GT(actual2, given) << "expected exp avg to be more than added value" ;
}

TEST(ExpAvgMeasurement, should_eventually_stabilize_at_last_value)
{
    ExpAvgMeasurement sut;
    for (int i=0; i<10000; i++)
    {
        sut.add(1000.0);
    }
    for (int i=0; i<10000; i++)
    {
        sut.add(1.0);
    }
    
    double given = 10.0;
    for (int i=0; i<10000; i++)
    {
        sut.add(given);
    }

    auto actual = sut.get();

    EXPECT_GT(actual, given - 0.01);
    EXPECT_LT(actual, given);
}
