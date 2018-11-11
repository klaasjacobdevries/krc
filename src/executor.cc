#include "executor.hh"
#include <cassert>
#include <channel.hh>
#include "debug.hh"
#include "single_executor.hh"

using namespace std;

namespace krc {
namespace {

class noop_executor final: public executor_impl
{
public:
    void dispatch(target_t) final
    {
        assert(false && "dispatch() called outside of run()");
    }

    void run(target_t) final
    {
        assert(false && "unreachable");
    }

    void yield() final
    {
        this_thread::yield();
    }

    routine_id get_id() const final
    {
        return no_routine_id;
    }
};

struct defer
{
    std::function<void()> func;
    ~defer()
    {
        func();
    }
};

}

executor executor::s_instance;

executor& executor::instance()
{
    return s_instance;
}

void executor::dispatch(target_t target)
{
    assert(d_delegate);
    d_delegate->dispatch(move(target));
}

void executor::run(target_t target, size_t num_threads)
{
    assert(get_id() == no_routine_id && "run() called from within an active run()");
    assert(num_threads > 0 && "need at least one thread");

    if(num_threads == 1)
        run_single(move(target));
    else
        run_multi(move(target), num_threads);
}

void executor::run_single(target_t target)
{
    defer reset{[this] {
        this->d_delegate = std::make_unique<noop_executor>();
    }};
    d_delegate = std::make_unique<single_executor>();
    d_delegate->run(move(target));
}

void executor::run_multi(target_t target, size_t num_threads)
{
    assert(num_threads >= 2);
}

void executor::yield()
{
    assert(d_delegate);
    d_delegate->yield();
}

routine_id executor::get_id() const
{
    assert(d_delegate);
    return d_delegate->get_id();
}

executor::executor()
    : d_delegate(std::make_unique<noop_executor>())
{}

} // namespace krc
