#pragma once
#include <type_traits>
namespace CTC {
template <typename T, auto L> struct CTVec {
  using size_type = ::std::remove_cvref_t<decltype(L)>;
  using value_type = T;
  constexpr static auto length = L;
  T data[L];
  constexpr auto &operator[](size_type i) { return data[i]; }
  constexpr const auto &operator[](size_type i) const { return data[i]; }
  constexpr auto begin() { return &data[0]; }
  constexpr const auto cbegin() const { return &data[0]; }
  constexpr auto end() { return &data[L]; }
  constexpr const auto cend() const { return &data[L]; }
  constexpr operator auto() { return data; }
  constexpr operator const auto() const { return data; }
};
template <typename T, typename... Ts>
CTVec(T first, Ts... rest) -> CTVec<std::remove_cvref_t<T>, sizeof...(Ts) + 1>;
template <typename T, auto L1, auto L2>
constexpr auto operator+(const CTVec<T, L1> &a, const CTVec<T, L2> &b) {
  CTVec<T, L1 + L2> res;
  for (auto i = 0; i < L1; i++) {
    res[i] = a[i];
  }
  for (auto i = 0; i < L2; i++) {
    res[L1 + i] = b[i];
  }
  return res;
}
} // namespace CTC
