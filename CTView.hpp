#pragma once
#include <concepts>

namespace CTC {
template <typename T>
concept Iterator = requires(std::remove_cvref_t<T> a, std::remove_cvref_t<T> b) {
    { a++ } -> std::same_as<std::remove_cvref_t<T>>;
    { *a };
    { a = b };
    { a == b } -> std::convertible_to<bool>;
};

template <Iterator T>
struct View {
    T                         begin_;
    T                         end_;
    [[nodiscard]] auto&       begin() { return begin_; }
    [[nodiscard]] const auto& begin() const { return begin_; }
    [[nodiscard]] auto&       end() { return end_; }
    [[nodiscard]] const auto& end() const { return end_; }
    auto                      begin(T b) {
        auto t = begin_;
        begin_ = b;
        return t;
    }
    auto end(T b) {
        auto t = end_;
        end_   = b;
        return t;
    }
};
template <typename T>
constexpr auto form_container(T& a) {
    return View{a.begin(), a.end()};
}

} // namespace CTC
