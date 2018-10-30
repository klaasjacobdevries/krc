#pragma once

#include <functional>

namespace krc {

typedef std::function<void()> target_t;

enum class context_method {
    UCONTEXT,
    THREADS,
    DEFAULT = context_method::UCONTEXT
};

template <context_method M = context_method::DEFAULT>
struct context;

template <>
struct context<context_method::UCONTEXT>
{
    typedef void *handle;

    handle make(const target_t &target, size_t stack_size);

    void swap(handle old_ctx, handle new_ctx);
};

}