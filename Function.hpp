#pragma once
#include "Any.hpp"
namespace CTC {
namespace details {
template <typename T>
struct GetFunctionType;
template <typename RetType, typename ObjType, typename... ArgsType>
struct GetFunctionType<RetType (ObjType::*)(ArgsType...) const> {
    using type = RetType(ArgsType...);
};
template <typename RetType, typename ObjType, typename... ArgsType>
struct GetFunctionType<RetType (ObjType::*)(ArgsType...) volatile> {
    using type = RetType(ArgsType...);
};
template <typename RetType, typename ObjType, typename... ArgsType>
struct GetFunctionType<RetType (ObjType::*)(ArgsType...) const volatile> {
    using type = RetType(ArgsType...);
};
template <typename RetType, typename ObjType, typename... ArgsType>
struct GetFunctionType<RetType (ObjType::*)(ArgsType...)> {
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
Function(T) -> Function<typename details::GetFunctionType<decltype(&T::operator())>::type>;
template <typename RetType, typename... ArgsType>
Function(RetType (*)(ArgsType...)) -> Function<RetType(ArgsType...)>;
} // namespace CTC
