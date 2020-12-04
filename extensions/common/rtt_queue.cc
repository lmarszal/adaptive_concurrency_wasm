#include "rtt_queue.hpp"
#include "proxy_wasm_common.h"

const char* RTT_QUEUE_NAME = "proxy.lmarszal.adaptive_concurrency.rtt_queue";

bool RttQueue::Dequeue(RttData& elem)
{
    WasmDataPtr data;
    auto res = dequeueSharedQueue(rtt_queue_token_, &data);
    if (res == WasmResult::Ok)
    {
        elem = RttData::fromString(data.get()->toString());
        return true;
    }
    return false;
}

void RttQueue::Enqueue(RttData elem)
{
    enqueueSharedQueue(rtt_queue_token_, elem.toString());
}

void RttQueue::Ensure(std::string vm_name)
{
    auto res = resolveSharedQueue(vm_name, RTT_QUEUE_NAME, &rtt_queue_token_);
    if (res != WasmResult::Ok)
    {
        registerSharedQueue(RTT_QUEUE_NAME, &rtt_queue_token_);
    }
}
