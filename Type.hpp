#pragma once
#include "CTCStr.hpp"
#include <utility>

namespace CTC {
namespace details {
template <typename T>
consteval auto name() {
#ifdef _MSC_VER
    return CTCStr{__FUNCSIG__};
#else
    return CTCStr{__PRETTY_FUNCTION__};
#endif
}
template <auto T>
consteval auto name() {
#ifdef _MSC_VER
    return CTCStr{__FUNCSIG__};
#else
    return CTCStr{__PRETTY_FUNCTION__};
#endif
}
constexpr auto typename_begin_index = [] {
    decltype(name<bool>())::size_type res = 0;
    for (auto c : name<bool>())
        if (c == 'b') return res;
        else res++;
    std::unreachable();
}();
constexpr auto unused_part_length = decltype(name<int>())::length - 3;
} // namespace details
template <typename T>
consteval auto name_of() {
    CTCStr<char, decltype(details::name<T>())::length - details::unused_part_length + 1> res = {};
    auto                                                                                 t   = details::name<T>();
    for (auto i = 0; i < decltype(res)::length; i++) res.data[i] = t.data[i + details::typename_begin_index];
    return res;
}
template <typename T>
consteval std::size_t hash_of() {
    std::size_t hash = 1024;
    for (std::size_t c : name_of<T>()) hash += (hash << 4) + c;
    return hash;
}
template <auto T>
consteval auto name_of() {
    CTCStr<char, decltype(details::name<T>())::length - details::unused_part_length + 1> res = {};
    auto                                                                                 t   = details::name<T>();
    for (auto i = 0; i < decltype(res)::length; i++) res.data[i] = t.data[i + details::typename_begin_index];
    return res;
}
template <auto T>
consteval std::size_t hash_of() {
    std::size_t hash = 1024;
    for (std::size_t c : name_of<T>()) hash += (hash << 4) + c;
    return hash;
}
namespace details {
template <std::size_t I>
struct Index {};
template <typename T>
struct Type {
    using type = T;
};
template <auto I, typename... Ts>
struct TypeContainerBase;
template <auto I, typename T, typename... Ts>
struct TypeContainerBase<I, T, Ts...> : TypeContainerBase<I + 1, Ts...> {
    using TypeContainerBase<I + 1, Ts...>::get;
    static auto get(Index<I>) { return Type<T>{}; }
};
template <auto I, typename T>
struct TypeContainerBase<I, T> {
    static auto get(Index<I>) { return Type<T>{}; }
};
template <typename T>
concept HasValueType = requires { typename T::value_type; };
} // namespace details
template <typename... Ts>
struct TypeContainer : details::TypeContainerBase<0, Ts...> {
    using details::TypeContainerBase<0, Ts...>::get;
    template <auto I>
    using type = decltype(get(details::Index<I>{}))::type;
};
template <>
struct TypeContainer<> {};
template <typename T>
struct value_type_of {
    using type = void;
};
template <details::HasValueType T>
struct value_type_of<T> {
    using type = typename T::value_type;
};
template <typename T>
using value_type_of_t = typename value_type_of<T>::type;
namespace details {
template <typename T>
struct FunctionInfoBase;
template <typename RetType, typename ObjType, typename... ArgsType>
struct FunctionInfoBase<RetType (ObjType::*)(ArgsType...) const> {
    using args                 = TypeContainer<ArgsType...>;
    constexpr static auto argc = sizeof...(ArgsType);
    using ret                  = RetType;
};
template <typename RetType, typename ObjType, typename... ArgsType>
struct FunctionInfoBase<RetType (ObjType::*)(ArgsType...) volatile> {
    constexpr static auto argc = sizeof...(ArgsType);
    using args                 = TypeContainer<ArgsType...>;
    using ret                  = RetType;
};
template <typename RetType, typename ObjType, typename... ArgsType>
struct FunctionInfoBase<RetType (ObjType::*)(ArgsType...) const volatile> {
    using args                 = TypeContainer<ArgsType...>;
    constexpr static auto argc = sizeof...(ArgsType);
    using ret                  = RetType;
};
template <typename RetType, typename ObjType, typename... ArgsType>
struct FunctionInfoBase<RetType (ObjType::*)(ArgsType...)> {
    using args                 = TypeContainer<ArgsType...>;
    constexpr static auto argc = sizeof...(ArgsType);
    using ret                  = RetType;
};
template <typename T>
using CallableObjectHelper = decltype(&T::operator());
template <typename T>
concept IsCallableObject = requires { typename CallableObjectHelper<T>; };
} // namespace details
template <typename T>
struct FunctionInfo;
template <details::IsCallableObject T>
struct FunctionInfo<T> {
    using args                 = details::FunctionInfoBase<details::CallableObjectHelper<T>>::args;
    constexpr static auto argc = details::FunctionInfoBase<details::CallableObjectHelper<T>>::argc;
    using ret                  = details::FunctionInfoBase<details::CallableObjectHelper<T>>::ret;
};
template <typename RetType, typename... ArgsType>
struct FunctionInfo<RetType (*)(ArgsType...)> {
    using args                 = TypeContainer<ArgsType...>;
    constexpr static auto argc = sizeof...(ArgsType);
    using ret                  = RetType;
};
template <typename RetType, typename... ArgsType>
struct FunctionInfo<RetType(ArgsType...)> {
    using args                 = TypeContainer<ArgsType...>;
    constexpr static auto argc = sizeof...(ArgsType);
    using ret                  = RetType;
};
template <typename RetType>
struct FunctionInfo<RetType (*)()> {
    using args                 = TypeContainer<>;
    constexpr static auto argc = 0;
    using ret                  = RetType;
};
template <typename RetType>
struct FunctionInfo<RetType()> {
    using args                 = TypeContainer<>;
    constexpr static auto argc = 0;
    using ret                  = RetType;
};
} // namespace CTC
