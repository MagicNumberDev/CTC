#pragma once
#include "Any.hpp"
#include "CTDArray.hpp"
#include "Type.hpp"
#include <functional>

namespace CTC {
namespace details {

template <auto I, auto Argc, typename T>
constexpr auto any_call_base(T& fn, CTDArray<Any>& args) -> Any {
    if constexpr (I < Argc) {
        auto tfn = std::bind_front(fn, args[I]);
        return any_call_base<I + 1, Argc>(tfn, args);
    } else {
        if constexpr (std::is_same_v<void, decltype(fn())>) {
            fn();
            return {};
        } else {
            return {fn()};
        }
        return {};
    }
}
template <typename T>
constexpr auto any_call(T& fn, CTDArray<Any>& args) {
    return any_call_base<0, FunctionInfo<T>::argc>(fn, args);
}
} // namespace details
struct AnyFunction {
    Any fn;
    Any (*call)(Any&, CTDArray<Any>&);
    constexpr AnyFunction() : fn(), call(nullptr) {}
    template <typename T>
    constexpr AnyFunction(const T& fn) : fn(fn) {
        call = [](Any& fn, CTDArray<Any>& args) -> Any {
            if (args.size() != CTC::FunctionInfo<T>::argc) {
                return {};
            } else {
                return details::any_call(fn.cast_to_ref<T>(), args);
            }
        };
    }
    constexpr Any operator()(CTDArray<Any>& args) { return call(fn, args); }
};
} // namespace CTC
