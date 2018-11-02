#include "runtime.hh"
#include "executor.hh"

namespace krc {

void dispatch(const std::function<void()>& target, size_t stack_size)
{
    auto& exec = executor::instance();
    exec.dispatch(target, stack_size);
}

void run(const std::function<void()>& target, size_t stack_size)
{
    auto& exec = executor::instance();
    exec.run(target, stack_size);
}

void yield()
{
    auto& exec = executor::instance();
    exec.yield();
}

routine_id get_id()
{
    auto& exec = executor::instance();
    return exec.get_id();
}

} // namespace krc
