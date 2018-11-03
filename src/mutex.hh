#pragma once

#include "no_copy.hh"
#include <atomic>

namespace krc {

class mutex : private no_copy
{
public:
    explicit mutex();

    void lock();

    void unlock();

    bool try_lock();

private:
    std::atomic_flag d_lock;
};

} // namespace krc
