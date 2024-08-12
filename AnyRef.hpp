#pragma once
#include "Type.hpp"
#include <type_traits>

namespace CTC {
struct AnyRef {
    void*       data;
    std::size_t hash;
    constexpr AnyRef() : data(nullptr), hash(hash_of<std::nullptr_t>()) {}
    constexpr AnyRef(const AnyRef&) = default;
    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, AnyRef>)
    constexpr AnyRef(T& data) : data(&data),
                                hash(hash_of<T>()) {}
    template <typename T>
    constexpr operator T&() {
        if (hash == hash_of<T>()) return *((T*)data);
        std::unreachable();
    }
    template <typename T>
    constexpr operator const T&() const {
        if (hash == hash_of<T>()) return *((T*)data);
        std::unreachable();
    }
};
struct ConstAnyRef {
    const void* data;
    std::size_t hash;
    constexpr ConstAnyRef() : data(nullptr), hash(hash_of<std::nullptr_t>()) {}
    constexpr ConstAnyRef(const ConstAnyRef&) = default;
    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, ConstAnyRef>)
    constexpr ConstAnyRef(const T& data) : data(&data),
                                           hash(hash_of<T>()) {}
    template <typename T>
    constexpr operator T&() {
        if (hash == hash_of<T>()) return *((T*)data);
        std::unreachable();
    }
    template <typename T>
    constexpr operator const T&() const {
        if (hash == hash_of<T>()) return *((T*)data);
        std::unreachable();
    }
};
} // namespace CTC