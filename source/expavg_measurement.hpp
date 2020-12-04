#pragma once

#include <stdint.h>
#include <string>
#include <cstdio>

class ExpAvgMeasurement
{
public:
    ExpAvgMeasurement()
    {
        value_ = 0;
        sum_ = 0;
        count_ = 0;
    }

    double add(double sample);
    void set(double value);
    double get();

    static ExpAvgMeasurement fromString(std::string str)
    {
        if (str.length() <= 0)
        {
            return ExpAvgMeasurement(0, 0, 0);
        }
        else
        {
            double value;
            double sum;
            uint32_t count;
            sscanf(str.c_str(), "%lf:%lf:%u", &value, &sum, &count);
            return ExpAvgMeasurement(value, sum, count);
        }
    }

    std::string toString()
    {
        char buffer[1024];
        sprintf(buffer, "%lf:%lf:%u", value_, sum_, count_);
        return std::string(buffer);
    }
private:
    ExpAvgMeasurement(double value, double sum, double count)
    {
        value_ = value;
        sum_ = sum;
        count_ = count;
    }

    uint32_t window = 600;
    uint32_t warmupWindow = 10;

    double value_;
    double sum_;
    uint32_t count_;
};


