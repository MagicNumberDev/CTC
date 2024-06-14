#pragma once
#include "CTCBase.hpp"
namespace CTC {
template <typename T, auto L>
struct CTCStr : basic_CTC_container {
    using size_type              = ::std::remove_cvref_t<decltype(L)>;
    using value_type             = T;
    constexpr static auto length = L - 1;
    T                     data[L];
    constexpr CTCStr()                         = default;
    constexpr CTCStr(const CTCStr&)            = default;
    constexpr CTCStr(CTCStr&&)                 = default;
    constexpr CTCStr& operator=(const CTCStr&) = default;
    constexpr CTCStr& operator=(CTCStr&&)      = default;
    constexpr CTCStr(const T (&s)[L]) {
        for (size_type i = 0; i < L; i++) data[i] = s[i];
    }
    constexpr auto&       operator[](size_type i) { return data[i]; }
    constexpr const auto& operator[](size_type i) const { return data[i]; }
    constexpr auto        begin() { return &data[0]; }
    constexpr const auto  cbegin() const { return &data[0]; }
    constexpr const auto  begin() const { return &data[0]; }
    constexpr auto        end() { return &data[L - 1]; }
    constexpr const auto  cend() const { return &data[L - 1]; }
    constexpr const auto  end() const { return &data[L - 1]; }
    constexpr operator auto() { return data; }
    constexpr operator const auto() const { return data; }
    constexpr const auto c_str() { return data; }
    template <size_type NewStringLength>
        requires(NewStringLength <= length)
    constexpr CTCStr<value_type, NewStringLength + 1> split(size_type s) {
        CTCStr<value_type, NewStringLength + 1> res = {};
        for (size_type i = 0; i < NewStringLength; i++) {
            res[i] = data[s + i];
        }
        return res;
    }
};
template <typename T, auto N>
CTCStr(T (&)[N]) -> CTCStr<std::remove_cvref_t<T>, N>;
template <typename T, auto L1, auto L2>
[[nodiscard]] constexpr auto operator+(const CTCStr<T, L1>& a, const CTCStr<T, L2>& b) {
    CTCStr<T, L1 + L2 - 1> res;
    for (auto i = 0; i < L1; i++) {
        res[i] = a[i];
    }
    for (auto i = 0; i < L2; i++) {
        res[L1 + i - 1] = b[i];
    }
    return res;
}
} // namespace CTC
