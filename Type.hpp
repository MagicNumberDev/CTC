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
constexpr auto unused_part_length = decltype(name<int>())::length - 4;
} // namespace details
template <typename T>
consteval auto name_of() {
    CTCStr<char, decltype(details::name<T>())::length - details::unused_part_length> res = {};
    auto                                                                             t   = details::name<T>();
    for (auto i = 0; i < decltype(res)::length; i++) {
        res.data[i] = t.data[i + details::typename_begin_index];
    }
    return res;
}
template <typename T>
consteval unsigned long long hash_of() {
    unsigned long long hash = 1024;
    for (unsigned long long c : name_of<T>()) hash += (hash << 4) + c;
    return hash;
}
template <auto T>
consteval auto name_of() {
    CTCStr<char, decltype(details::name<T>())::length - details::unused_part_length> res = {};
    auto                                                                             t   = details::name<T>();
    for (auto i = 0; i < decltype(res)::length; i++) {
        res.data[i] = t.data[i + details::typename_begin_index];
    }
    return res;
}
template <auto T>
consteval unsigned long long hash_of() {
    unsigned long long hash = 1024;
    for (unsigned long long c : name_of<T>()) hash += (hash << 4) + c;
    return hash;
}
} // namespace CTC
