#pragma once
#include "Type.hpp"
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace AnyObject {
using id_t = unsigned long long;
template <typename T>
id_t id_of = CTC::hash_of<T>();

struct Object {
private:
    static inline std::unordered_set<id_t>                         registered = {id_of<nullptr_t>};
    static inline std::unordered_map<id_t, void (*)(void*)>        deleters;
    static inline std::unordered_map<id_t, void* (*)(void*)>       copiers;
    static inline std::unordered_map<id_t, bool (*)(void*, void*)> comparators;
    static inline std::unordered_map<void*, int>                   refcounts;
    id_t                                                           id;
    void*                                                          data;
    Object(id_t id, void* data) : id(id), data(data) { refcounts[data] = 1; }

public:
    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, Object>)
    Object(const T& data) : id(id_of<T>),
                            data(new T(data)) {
        if (!registered.contains(id)) {
            registered.insert(id);
            if constexpr (requires(T* a) { delete a; }) deleters[id] = [](void* v) -> void { delete (T*)v; };
            if constexpr (requires(T a) { new T(a); }) copiers[id] = [](void* v) -> void* { return new T(*((T*)v)); };
            if constexpr (requires(T a, T b) { a == b; })
                comparators[id] = [](void* a, void* b) -> bool { return (*((T*)a)) == (*((T*)b)); };
        }
        refcounts[this->data] = 1;
    }
    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, Object> && !std::is_reference_v<T>)
    Object(T&& data) : id(id_of<T>),
                       data(new T(std::move(data))) {
        if (!registered.contains(id)) {
            if constexpr (requires(T* a) { delete a; }) deleters[id] = [](void* v) -> void { delete (T*)v; };
            if constexpr (requires(T a) { new T(a); }) copiers[id] = [](void* v) -> void* { return new T(*((T*)v)); };
            if constexpr (requires(T a, T b) { a == b; })
                comparators[id] = [](void* a, void* b) -> bool { return (*((T*)a)) == (*((T*)b)); };
        }
        refcounts[this->data] = 1;
    }
    ~Object() {
        if (refcounts.contains(data)) {
            refcounts[data]--;
            if (refcounts[data] <= 0) {
                if (deleters.contains(id)) {
                    deleters[id](data);
                }
                refcounts.erase(data);
            }
        }
    }
    Object ref() {
        refcounts[data]++;
        return Object{id, data};
    }
    Object copy() {
        if (copiers.contains(id)) {
            return {id, copiers[id](data)};
        } else {
            return {id_of<std::nullptr_t>, nullptr};
        }
    }
    Object move() {
        auto tid   = id;
        auto tdata = data;
        id         = id_of<std::nullptr_t>;
        data       = nullptr;
        return {tid, tdata};
    }
    template <typename T>
    T* cast() {
        if (id_of<T> == id && refcounts.contains(data)) return static_cast<T*>(data);
        else return nullptr;
    }
    Object()              = delete;
    Object(const Object&) = delete;
    Object(Object&&)      = delete;
};
} // namespace AnyObject
