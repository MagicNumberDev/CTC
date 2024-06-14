#pragma once
#include <utility>
namespace CTC {
struct Any {
    template <typename T>
    static consteval unsigned long long hash() {
        unsigned long long hash = 1024;
#ifdef _MSC_VER
        for (unsigned long long c : __FUNCSIG__) hash += (hash << 4) + c;
#else
        for (unsigned long long c : __PRETTY_FUNCTION__) hash += (hash << 4) + c;
#endif
        return hash;
    }
    void*              data;
    unsigned long long type_hash;
    void (*destructor)(void*);
    void* (*copyConstructor)(void*);
    constexpr Any() : data(nullptr), type_hash(hash<std::nullptr_t>()), destructor(nullptr), copyConstructor(nullptr) {}
    constexpr ~Any() {
        if (data) destructor(data);
    }
    constexpr Any(const Any& o)
    : data(o.data == nullptr ? nullptr : o.copyConstructor(o.data)),
      type_hash(o.type_hash),
      destructor(o.destructor),
      copyConstructor(o.copyConstructor) {}
    constexpr Any(Any&& o)
    : data(o.data),
      type_hash(o.type_hash),
      destructor(o.destructor),
      copyConstructor(o.copyConstructor) {
        o.data = nullptr;
    }
    template <typename T>
        requires(!(std::same_as<T, Any&> || std::same_as<T, Any>))
    constexpr Any(T&& d)
    : data(new T(std::move(d))),
      type_hash(hash<T>()),
      destructor([](void* data) { delete ((T*)data); }),
      copyConstructor([](void* data) -> void* { return new T(*((T*)data)); }) {}
    template <typename T>
        requires(!(std::same_as<T, Any&> || std::same_as<T, Any>))
    constexpr Any(T& d)
    : data(new T(d)),
      type_hash(hash<T>()),
      destructor([](void* data) { delete ((T*)data); }),
      copyConstructor([](void* data) -> void* { return new T(*((T*)data)); }) {}
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
        data            = o.data == nullptr ? nullptr : o.copyConstructor(o.data);
        type_hash       = o.type_hash;
        destructor      = o.destructor;
        copyConstructor = o.copyConstructor;
        return *this;
    }
    constexpr auto& operator=(Any&& o) {
        data            = o.data;
        type_hash       = o.type_hash;
        destructor      = o.destructor;
        copyConstructor = o.copyConstructor;
        o.data          = nullptr;
        return *this;
    }
};
} // namespace CTC
