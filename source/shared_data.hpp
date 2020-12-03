#pragma once

#include <string>
#include <stdint.h>

enum SharedDataSetResult : uint32_t
{
    Ok = 0,
    CasMismatch
};

struct ExpAvgMeasurementState
{
    double value;
    double sum;
    uint32_t count;
};

class SharedInt
{
    public:
        SharedInt(const char* key)
        {
            this->key_ = std::string(key);
        }

        uint32_t load();
        void set(uint32_t data);
        uint32_t fetch_add(uint32_t i);
        uint32_t fetch_sub(uint32_t i);

    protected:
        std::string key_;
        uint32_t value = 0; // FOR MOCK ONLY
};

class SharedExpAvgMeasurementState
{
    public:
        SharedExpAvgMeasurementState(const char* key)
        {
            this->key_ = std::string(key);
            this->value = (struct ExpAvgMeasurementState){.value = 0, .sum = 0, .count = 0};
        }

        ExpAvgMeasurementState load(uint32_t* cas);
        SharedDataSetResult set(ExpAvgMeasurementState data, uint32_t cas);

    protected:
        std::string key_;
        ExpAvgMeasurementState value; // FOR MOCK ONLY
};
