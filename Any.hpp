#pragma once
#include "Type.hpp"
#include <utility>

namespace CTC {
struct Any {
    template <typename T>
    static consteval unsigned long long hash() {
        return hash_of<T>();
    }
    void*              data;
    unsigned long long type_hash;
    void (*deleter)(void*);
    void* (*copier)(void*);
    constexpr Any() : data(nullptr), type_hash(hash<std::nullptr_t>()), deleter(nullptr), copier(nullptr) {}
    constexpr ~Any() {
        if (data) deleter(data);
    }
    constexpr Any(const Any& o)
    : data(o.data == nullptr ? nullptr : o.copier(o.data)),
      type_hash(o.type_hash),
      deleter(o.deleter),
      copier(o.copier) {}
    constexpr Any(Any&& o) : data(o.data), type_hash(o.type_hash), deleter(o.deleter), copier(o.copier) {
        o.data = nullptr;
    }
    template <typename T>
        requires(!(std::same_as<T, Any&> || std::same_as<T, Any>))
    constexpr Any(T&& d)
    : data(new T(std::move(d))),
      type_hash(hash<T>()),
      deleter([](void* data) { delete ((T*)data); }),
      copier([](void* data) -> void* { return new T(*((T*)data)); }) {}
    template <typename T>
        requires(!(std::same_as<T, Any&> || std::same_as<T, Any>))
    constexpr Any(T& d)
    : data(new T(d)),
      type_hash(hash<T>()),
      deleter([](void* data) { delete ((T*)data); }),
      copier([](void* data) -> void* { return new T(*((T*)data)); }) {}
    template <typename T>
    constexpr operator T*() {
        if (hash<T>() == type_hash) return (T*)data;
        else return nullptr;
    }
    template <typename T>
    constexpr operator T&() {
        if (hash<T>() == type_hash) return *((T*)data);
        std::unreachable();
    }
    template <typename T>
    constexpr operator const T*() const {
        if (hash<T>() == type_hash) return (T*)data;
        else return nullptr;
    }
    template <typename T>
    constexpr operator const T&() const {
        if (hash<T>() == type_hash) return *((T*)data);
        std::unreachable();
    }
    template <typename T>
    constexpr T* cast_to_pointer() {
        if (hash<T>() == type_hash) return (T*)data;
        else return nullptr;
    }
    template <typename T>
    constexpr T& cast_to_ref() {
        if (hash<T>() == type_hash) return *((T*)data);
        std::unreachable();
    }
    template <typename T>
    constexpr const T* cast_to_pointer() const {
        if (hash<T>() == type_hash) return (T*)data;
        else return nullptr;
    }
    template <typename T>
    constexpr const T& cast_to_ref() const {
        if (hash<T>() == type_hash) return *((T*)data);
        std::unreachable();
    }
    template <typename T>
    constexpr bool is_type() {
        return type_hash == hash<T>();
    }
    constexpr auto& operator=(const Any& o) {
        data      = o.data == nullptr ? nullptr : o.copier(o.data);
        type_hash = o.type_hash;
        deleter   = o.deleter;
        copier    = o.copier;
        return *this;
    }
    constexpr auto& operator=(Any&& o) {
        data      = o.data;
        type_hash = o.type_hash;
        deleter   = o.deleter;
        copier    = o.copier;
        o.data    = nullptr;
        return *this;
    }
};
} // namespace CTC
