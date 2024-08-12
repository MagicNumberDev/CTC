#pragma once
#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstdlib>
#include <type_traits>

template <auto condition, auto ifTrue, auto ifFalse>
constexpr auto switchValue = condition ? ifTrue : ifFalse;
template <typename CharT>
concept IsChar =
    ::std::same_as<::std::remove_cv_t<CharT>, char> || ::std::same_as<::std::remove_cv_t<CharT>, wchar_t>
    || ::std::same_as<::std::remove_cv_t<CharT>, char8_t> || ::std::same_as<::std::remove_cv_t<CharT>, char16_t>
    || ::std::same_as<::std::remove_cv_t<CharT>, char32_t>;
template <typename T, ::std::size_t L>
struct CVec {
    T             data[L];
    ::std::size_t length = L;
    using value_type     = T;
    template <typename U>
        requires ::std::convertible_to<T, U>
    constexpr operator CVec<U, L>() const {
        CVec<U, L> res;
        for (::std::size_t i = 0; i < L; i++) res.data[i] = data[i];
        return res;
    }
    constexpr                   operator auto() { return data; }
    constexpr                   operator const auto() const { return data; }
    constexpr const value_type& operator[](::std::size_t i) const { return data[i]; }
    constexpr value_type&       operator[](::std::size_t i) { return data[i]; }
    constexpr value_type*       begin() { return &data[0]; }
    constexpr value_type*       end() { return &data[switchValue<IsChar<::std::remove_cv_t<value_type>>, L - 1, L>]; }
    constexpr const value_type* begin() const { return &data[0]; }
    constexpr const value_type* end() const {
        return &data[switchValue<IsChar<::std::remove_cv_t<value_type>>, L - 1, L>];
    }
};
template <typename T, typename... Ts>
CVec(T first, Ts... rest) -> CVec<::std::remove_cv_t<T>, sizeof...(Ts) + 1>;
template <typename T, ::std::size_t N>
CVec(T (&)[N]) -> CVec<::std::remove_cv_t<T>, N>;
template <typename T, ::std::size_t L1, typename U, ::std::size_t L2>
constexpr bool operator==(const CVec<T, L1>& a, const CVec<U, L2>& b) {
    if constexpr (L1 != L2) return false;
    else if (::std::convertible_to<::std::remove_cv_t<U>, ::std::remove_cv_t<T>>
             && !::std::same_as<::std::remove_cv_t<U>, ::std::remove_cv_t<T>>) {
        auto t = static_cast<CVec<::std::remove_cv_t<T>, L1>>(b);
        for (::std::size_t i = 0; i < L1; i++)
            if (a[i] != t[i]) return false;
    } else if (::std::convertible_to<::std::remove_cv_t<T>, ::std::remove_cv_t<U>>
               && !::std::same_as<::std::remove_cv_t<T>, ::std::remove_cv_t<U>>) {
        auto t = static_cast<CVec<::std::remove_cv_t<U>, L2>>(a);
        for (::std::size_t i = 0; i < L1; i++)
            if (t[i] != b[i]) return false;
    } else if (!::std::same_as<::std::remove_cv_t<T>, ::std::remove_cv_t<U>>) return false;
    else {
        for (::std::size_t i = 0; i < L1; i++)
            if (a[i] != b[i]) return false;
    }
    return true;
}

template <typename T, ::std::size_t L1, typename U, ::std::size_t L2>
constexpr CVec<T, switchValue<IsChar<::std::remove_cv_t<T>> || IsChar<::std::remove_cv_t<U>>, L1 + L2 - 1, L1 + L2>>
operator+(const CVec<T, L1>& a, const CVec<U, L2>& b) {
    CVec<T, switchValue<IsChar<::std::remove_cv_t<T>> || IsChar<::std::remove_cv_t<U>>, L1 + L2 - 1, L1 + L2>> res;
    for (::std::size_t i = 0;
         i < switchValue < IsChar<::std::remove_cv_t<T>> || IsChar<::std::remove_cv_t<U>>, L1 - 1, L1 > ;
         i++)
        res[i] = a[i];
    for (::std::size_t i = 0; i < L2; i++)
        res[i + switchValue < IsChar<::std::remove_cv_t<T>> || IsChar<::std::remove_cv_t<U>>, L1 - 1, L1 > ] = b[i];
    return res;
}
