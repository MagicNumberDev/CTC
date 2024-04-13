#pragma once
#include "CTCBase.hpp"
#include <compare>
#include <type_traits>

namespace CTC {
namespace details {
template <typename T>
concept CTCC = std::is_base_of_v<basic_CTC_container, T>;
}
template <details::CTCC T, details::CTCC U>
constexpr std::weak_ordering operator==(const T& a, const U& b) {
    auto ai = a.cbegin();
    auto bi = b.cbegin();
    for (; ai != a.cend() && bi != b.cend() && *ai == *bi; ai++, bi++)
        ;
    if (ai == a.cend() && bi == b.cend()) return std::weak_ordering::equivalent;
    if (*ai > *bi) return std::weak_ordering::greater;
    else return std::weak_ordering::less;
}
} // namespace CTC