#pragma once
#include <type_traits>
namespace CTC {
struct basic_CTC_container {};
namespace details {
template <typename T>
concept CTCC = std::is_base_of_v<basic_CTC_container, T>;
} // namespace details
} // namespace CTC
