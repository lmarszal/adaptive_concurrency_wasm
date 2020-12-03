#include "proxy_wasm_intrinsics.h"
#include "shared_data.hpp"
#include <cstdlib>
#include <string>
#include "base64.hpp"

uint32_t uint32_load_cas(std::string key, uint32_t* cas = nullptr)
{
    WasmDataPtr data;
    auto res = getSharedData(key, &data, cas);
    if (res != WasmResult::Ok)
    {
        return 0;
    }
    auto str = data.get()->view();
    return std::atoi(str.cbegin());
}

uint32_t SharedInt::load()
{
    return uint32_load_cas(key_);
}

void SharedInt::set(uint32_t data)
{
    char buffer[128];
    sprintf(buffer, "%u", data);
    setSharedData(key_, buffer);
}

uint32_t SharedInt::fetch_add(uint32_t i)
{
    WasmResult res;
    uint32_t data;
    do
    {
        uint32_t cas;
        data = uint32_load_cas(key_, &cas);
        data += i;
        char buffer[128];
        sprintf(buffer, "%u", data);
        res = setSharedData(key_, buffer, cas);
    } while (res == WasmResult::CasMismatch);
    return data;
}

uint32_t SharedInt::fetch_sub(uint32_t i)
{
    WasmResult res;
    uint32_t data;
    do
    {
        uint32_t cas;
        data = uint32_load_cas(key_, &cas);
        data -= i;
        char buffer[128];
        sprintf(buffer, "%u", data);
        res = setSharedData(key_, buffer, cas);
    } while (res == WasmResult::CasMismatch);
    return data;
}

// TODO use json (protobuf?) instead of base64

ExpAvgMeasurementState SharedExpAvgMeasurementState::load(uint32_t* cas)
{
    WasmDataPtr data;
    auto res = getSharedData(key_, &data, cas);
    if (res != WasmResult::Ok)
    {
        return (struct ExpAvgMeasurementState){.value = 0, .sum = 0, .count = 0};
    }
    auto str = data.get()->view();
    ExpAvgMeasurementState state;
    boost::beast::detail::base64::decode(&state, data.get()->data(), data.get()->size());
    return state;
}

SharedDataSetResult SharedExpAvgMeasurementState::set(ExpAvgMeasurementState data, uint32_t cas)
{
    std::string dest;
    size_t len = sizeof(ExpAvgMeasurementState);
    dest.resize(boost::beast::detail::base64::encoded_size(len));
    dest.resize(boost::beast::detail::base64::encode(&dest[0], &data, len));

    auto res = setSharedData(key_, dest, cas);
    if (res == WasmResult::CasMismatch)
    {
        return SharedDataSetResult::CasMismatch;
    }
    return SharedDataSetResult::Ok;
}
