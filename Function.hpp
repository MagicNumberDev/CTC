#pragma once
#include "Any.hpp"
namespace CTC {
namespace details {
template <typename T>
struct FunctionType;
template <typename RetType, typename ObjType, typename... ArgsType>
struct FunctionType<RetType (ObjType::*)(ArgsType...) const> {
    using type = RetType(ArgsType...);
};
} // namespace details
template <typename T>
struct Function;
template <typename RetType, typename... ArgsType>
struct Function<RetType(ArgsType...)> {
    Any fn;
    RetType (*call)(Any&, ArgsType...);
    template <typename T>
    Function(T f) : fn(f) {
        call = [](Any& fn, ArgsType... args) { return (fn.cast_to_ref<T>())(args...); };
    }
    auto operator()(ArgsType&&... args) { return call(fn, args...); }
};
template <typename T>
Function(T) -> Function<typename details::FunctionType<decltype(&T::operator())>::type>;
template <typename RetType, typename... ArgsType>
Function(RetType (*)(ArgsType...)) -> Function<RetType(ArgsType...)>;
} // namespace CTC
