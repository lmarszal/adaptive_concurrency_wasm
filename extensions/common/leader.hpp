#pragma once

#include "proxy_wasm_intrinsics.h"
#include <cstdlib>

class LeaderTokenLease
{
public:
    LeaderTokenLease()
    {
        std::srand(getCurrentTimeNanoseconds());
        unique_id_ = std::rand();
    }

    bool isExpired(uint64_t now_ns);
    bool tryAckquire(uint64_t now_ns, uint64_t lease_time_ns);
    void extend(uint64_t now_ns, uint64_t lease_time_ns);
    bool isMine();
private:
    uint32_t unique_id_;
};
