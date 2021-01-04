#include "state.hpp"
#include <sstream>
#include "proxy_wasm_intrinsics.h"

const char* EXP_AVG_MEASUREMENT_STATE_NAME = "proxy.lmarszal.adaptive_concurrency.expavg.state";

std::string getExpAvgState()
{
    WasmDataPtr data;
    auto res = getSharedData(EXP_AVG_MEASUREMENT_STATE_NAME, &data);
    if (res == WasmResult::NotFound)
    {
        return std::string();
    }
    else if (res == WasmResult::Ok)
    {
        return data->toString();
    }
    else
    {
        // according to docs shouldn't happen
        abort();
    }
}

void setExpAvgState(std::string state)
{
    WasmDataPtr data;
    setSharedData(EXP_AVG_MEASUREMENT_STATE_NAME, state);
}

const char* LIMIT_STATE_NAME = "proxy.lmarszal.adaptive_concurrency.limit";

void ensureLimit()
{
    WasmDataPtr data;
    auto res = getSharedData(LIMIT_STATE_NAME, &data);
    if (res == WasmResult::NotFound)
    {
        setSharedData(LIMIT_STATE_NAME, "0");
    }
}

uint32_t getLimit()
{
    WasmDataPtr data;
    auto res = getSharedData(LIMIT_STATE_NAME, &data);
    if (res == WasmResult::NotFound)
    {
        LOG_ERROR("unexpected error: limit is not in state; fail_open");
        return 0;
    }
    else if (res == WasmResult::Ok)
    {
        uint32_t limit;
        sscanf(data->toString().c_str(), "%u", &limit);
        return limit;
    }
    else
    {
        // according to docs shouldn't happen
        abort();
    }
}

void setLimit(uint32_t limit)
{
    setSharedData(LIMIT_STATE_NAME, std::to_string(limit));
}

const char* INFLIGHT_STATE_NAME = "proxy.lmarszal.adaptive_concurrency.inflight";

void ensureInflight()
{
    WasmDataPtr data;
    auto res = getSharedData(INFLIGHT_STATE_NAME, &data);
    if (res == WasmResult::NotFound)
    {
        setSharedData(INFLIGHT_STATE_NAME, "0");
    }
}

int32_t addInflight(int32_t add)
{
    WasmDataPtr data;
    uint32_t cas;
    int32_t inflight;

    do
    {
        auto res = getSharedData(INFLIGHT_STATE_NAME, &data, &cas);
        if (res != WasmResult::Ok)
        {
            LOG_ERROR("unexpected error: inflight is not in state; fail_open");
            setSharedData(LIMIT_STATE_NAME, "0");
            return 1;
        }
        
        sscanf(data->toString().c_str(), "%d", &inflight);
        inflight += add;

        res = setSharedData(INFLIGHT_STATE_NAME, std::to_string(inflight), cas);
        if (res == WasmResult::Ok)
        {
            // hurrey!
            break;
        }
        else if (res == WasmResult::CasMismatch)
        {
            // someone was faster
        }
        else
        {
            // according to docs shouldn't happen
            abort();
        }
    } while (true);

    return inflight;
}

int32_t incrementInflight()
{
    return addInflight(1);
}

int32_t decrementInflight()
{
    return addInflight(-1);
}

const char* SHARED_STATE_FLAG_NAME = "proxy.lmarszal.adaptive_concurrency.shared_state_loaded";

bool getSharedStateLoaded()
{
    WasmDataPtr data;
    auto res = getSharedData(SHARED_STATE_FLAG_NAME, &data);
    return res == WasmResult::Ok;
}

void setSharedStateLoaded()
{
    setSharedData(SHARED_STATE_FLAG_NAME, "TRUE");
}
