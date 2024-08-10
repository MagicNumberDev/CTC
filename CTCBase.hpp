#pragma once
#include <type_traits>
namespace CTC {
struct basic_CTC_container {};
namespace details {
template <typename T>
concept CTCC = std::is_base_of_v<basic_CTC_container, T>;
template <auto I>
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
} // namespace details
template <typename... Ts>
struct TypeContainer : details::TypeContainerBase<0, Ts...> {
    using details::TypeContainerBase<0, Ts...>::get;
    template <auto I>
    using type = decltype(get(details::Index<I>{}))::type;
};
} // namespace CTC
