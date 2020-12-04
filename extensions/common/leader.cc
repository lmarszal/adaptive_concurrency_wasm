#include "leader.hpp"

#include <string>
#include <cstdio>

class Lease
{
public:
    bool isMine(uint32_t owner_id)
    {
        return owner_id == owner_id_;
    }

    bool isExpired(uint64_t now)
    {
        return lease_end_ <= 0 || lease_end_ < now;
    }

    static Lease New(uint64_t now, uint64_t lease_time, uint32_t owner_id)
    {
        return Lease(now + lease_time, owner_id);
    }

    static Lease Empty()
    {
        return Lease(0, 0);
    }

    static Lease fromString(std::string str)
    {
        uint64_t lease_end;
        uint32_t owner_id;
        sscanf(str.c_str(), "%llu:%u", &lease_end, &owner_id);
        return Lease(lease_end, owner_id);
    }

    std::string toString()
    {
        char buffer[1024];
        sprintf(buffer, "%llu:%u", lease_end_, owner_id_);
        return std::string(buffer);
    }
private:
    Lease(uint64_t lease_end, uint32_t owner_id)
    {
        lease_end_ = lease_end;
        owner_id_ = owner_id;
    }
    uint64_t lease_end_;
    uint32_t owner_id_;
};

const char* LEASE_NAME = "proxy.lmarszal.adaptive_concurrency.leader";

bool _try_acquire_lease(uint64_t owner_id, uint64_t now_ns, uint64_t lease_time_ns)
{
    Lease lease = Lease::Empty();

    do
    {
        WasmDataPtr data;
        uint32_t cas = 0;
        
        auto res = getSharedData(LEASE_NAME, &data, &cas);
        if (res == WasmResult::NotFound)
        {
            lease = Lease::Empty();
        }
        else if (res == WasmResult::Ok)
        {
            lease = Lease::fromString(data.get()->toString());
        }
        else
        {
            // according to docs shouldn't happen
            abort();
        }

        // Try acquire lease
        if (lease.isExpired(now_ns))
        {
            lease = Lease::New(now_ns, lease_time_ns, owner_id);
            res = setSharedData(LEASE_NAME, lease.toString(), cas);
            if (res == WasmResult::Ok)
            {
                // hurrey!
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
        }
        else
        {
            // got up-to-date lease
            break;
        }
        
    } while (true);

    return lease.isMine(owner_id);
}

Lease _get_lease()
{
    WasmDataPtr data;
    
    auto res = getSharedData(LEASE_NAME, &data);
    if (res == WasmResult::NotFound)
    {
        return Lease::Empty();
    }
    else if (res == WasmResult::Ok)
    {
        return Lease::fromString(data.get()->toString());
    }
    else
    {
        // according to docs shouldn't happen
        abort();
    }
}

void _set_lease(uint64_t owner_id, uint64_t now_ns, uint64_t lease_time_ns)
{
    auto lease = Lease::New(now_ns, lease_time_ns, owner_id);
    auto res = setSharedData(LEASE_NAME, lease.toString());
    if (res == WasmResult::Ok)
    {
        // hurrey!
    }
    else
    {
        // according to docs shouldn't happen
        abort();
    }
}

bool LeaderTokenLease::isExpired(uint64_t now_ns)
{
    return _get_lease().isExpired(now_ns);
}

bool LeaderTokenLease::tryAckquire(uint64_t now_ns, uint64_t lease_time_ns)
{
    return _try_acquire_lease(unique_id_, now_ns, lease_time_ns);
}

void LeaderTokenLease::extend(uint64_t now_ns, uint64_t lease_time_ns)
{
    _set_lease(unique_id_, now_ns, lease_time_ns);
}

bool LeaderTokenLease::isMine()
{
    return _get_lease().isMine(unique_id_);
}
