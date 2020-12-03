#include <cstdlib>
#include "../../source/shared_data.hpp"

uint32_t SharedInt::load()
{
    return value;
}

void SharedInt::set(uint32_t data)
{
    value = data;
}

uint32_t SharedInt::fetch_add(uint32_t i)
{
    return value += i;
}

uint32_t SharedInt::fetch_sub(uint32_t i)
{
    return value -= i;
}

ExpAvgMeasurementState SharedExpAvgMeasurementState::load(uint32_t* cas)
{
    return value;
}

SharedDataSetResult SharedExpAvgMeasurementState::set(ExpAvgMeasurementState data, uint32_t cas)
{
    value = data;
    return SharedDataSetResult::Ok;
}
