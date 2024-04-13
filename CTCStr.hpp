#pragma once
#include <type_traits>
namespace CTC {
template <typename T, auto L> struct CTCStr {
  using size_type = ::std::remove_cvref_t<decltype(L)>;
  using value_type = T;
  constexpr static auto length = L;
  T data[L];
  constexpr auto &operator[](size_type i) { return data[i]; }
  constexpr const auto &operator[](size_type i) const { return data[i]; }
  constexpr auto begin() { return &data[0]; }
  constexpr const auto cbegin() const { return &data[0]; }
  constexpr auto end() { return &data[L - 1]; }
  constexpr const auto cend() const { return &data[L - 1]; }
  constexpr operator auto() { return data; }
  constexpr operator const auto() const { return data; }
  constexpr const auto c_str() { return data; }
};
template <typename T, auto N>
CTCStr(T (&)[N]) -> CTCStr<std::remove_cvref_t<T>, N>;
template <typename T, auto L1, auto L2>
constexpr auto operator+(const CTCStr<T, L1> &a, const CTCStr<T, L2> &b) {
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
