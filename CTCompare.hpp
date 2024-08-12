#pragma once
#include "CTCBase.hpp"
#include <compare>

namespace CTC {
template <details::CTCC T, details::CTCC U>
[[nodiscard]] constexpr std::strong_ordering operator<=>(const T& a, const U& b) {
    auto ai = a.cbegin();
    auto bi = b.cbegin();
    for (; ai != a.cend() && bi != b.cend() && *ai == *bi; ai++, bi++);
    if (ai == a.cend() && bi == b.cend()) return std::strong_ordering::equivalent;
    if (*ai > *bi) return std::strong_ordering::greater;
    else return std::strong_ordering::less;
}
template <details::CTCC T, details::CTCC U>
[[nodiscard]] constexpr bool operator==(const T& a, const U& b) {
    auto ai = a.cbegin();
    auto bi = b.cbegin();
    for (; ai != a.cend() && bi != b.cend() && *ai == *bi; ai++, bi++);
    if (ai == a.cend() && bi == b.cend()) return true;
    else return false;
}
} // namespace CTC
