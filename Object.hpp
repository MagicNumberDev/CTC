#pragma once
#include "Type.hpp"
#include <format>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace AnyObject {
using id_t = decltype(CTC::hash_of<void>());
template <typename T>
constexpr id_t id_of = CTC::hash_of<T>();

struct Object {
    enum class CastRuleType { BigToSmall, SmallToBig, Equal };

private:
    static inline std::unordered_set<id_t>                         registered = {id_of<nullptr_t>};
    static inline std::unordered_map<id_t, void (*)(void*)>        deleters;
    static inline std::unordered_map<id_t, void* (*)(void*)>       copiers;
    static inline std::unordered_map<id_t, bool (*)(void*, void*)> comparators;
    static inline std::unordered_map<void*, int>                   refcounts;
    static inline std::unordered_map<
        std::pair<id_t, id_t>,
        std::pair<void* (*)(const void*), CastRuleType>,
        decltype([](const auto& k) { return (k.first << 32) + ((k.second << 32) >> 32); })>
          castrules;
    id_t  id;
    void* data;
    Object(id_t id, void* data) : id(id), data(data) { refcounts[data] = 1; }
    void* try_cast(id_t from, id_t to, void* val, std::unordered_set<id_t>& vis, int* depth = nullptr) {
        if (castrules.contains({from, to})) {
            return castrules[{from, to}].first(val);
        }
        vis.insert(from);
        int   min_depth = std::numeric_limits<int>::max();
        void* res       = nullptr;
        for (const auto& [k, c] : castrules) {
            const auto& [from_, to_] = k;
            if (from == from_ && (c.second == CastRuleType::Equal || c.second == CastRuleType::SmallToBig)
                && !vis.contains(to_)) {
                int  current_depth = 0;
                auto t             = c.first(val);
                auto r             = try_cast(to_, to, t, vis, &current_depth);
                deleters[to_](t);
                if (r) {
                    if (current_depth < min_depth) {
                        min_depth = current_depth;
                        if (res) deleters[to](res);
                        res = r;
                    } else {
                        deleters[to](r);
                    }
                }
            }
        }
        if (res) {
            if (depth) *depth = min_depth;
            return res;
        }
        min_depth = std::numeric_limits<int>::max();
        for (const auto& [k, c] : castrules) {
            const auto& [from_, to_] = k;
            if (from == from_ && c.second == CastRuleType::BigToSmall && !vis.contains(to_)) {
                int  current_depth = 0;
                auto t             = c.first(val);
                auto r             = try_cast(to_, to, t, vis, &current_depth);
                deleters[to_](t);
                if (r) {
                    if (current_depth < min_depth) {
                        min_depth = current_depth;
                        if (res) deleters[to](res);
                        res = r;
                    } else {
                        deleters[to](r);
                    }
                }
            }
        }
        if (res) {
            if (depth) *depth = min_depth;
            return res;
        }
        return nullptr;
    }
    template <typename F, typename T, CastRuleType type>
    static void register_normal_cast_rule() {
        register_normal_type<F>();
        register_normal_type<T>();
        register_cast_rule(
            id_of<F>,
            id_of<T>,
            [](const void* from) -> void* { return new T(*static_cast<const F*>(from)); },
            type
        );
        if constexpr (type == CastRuleType::BigToSmall && requires {
                          F(*static_cast<const T*>(nullptr));
                      } && !std::is_const_v<F>) {
            register_cast_rule(
                id_of<T>,
                id_of<F>,
                [](const void* from) -> void* { return new F(*static_cast<const T*>(from)); },
                CastRuleType::SmallToBig
            );
        }
        if constexpr (type == CastRuleType::SmallToBig && requires {
                          F(*static_cast<const T*>(nullptr));
                      } && !std::is_const_v<F>) {
            register_cast_rule(
                id_of<T>,
                id_of<F>,
                [](const void* from) -> void* { return new F(*static_cast<const T*>(from)); },
                CastRuleType::BigToSmall
            );
        }
        if constexpr (type == CastRuleType::Equal && requires {
                          F(*static_cast<const T*>(nullptr));
                      } && !std::is_const_v<F>) {
            register_cast_rule(
                id_of<T>,
                id_of<F>,
                [](const void* from) -> void* { return new F(*static_cast<const T*>(from)); },
                CastRuleType::Equal
            );
        }
    };


public:
    id_t  get_id() { return id; }
    void* get_data() { return data; }
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
    template <typename T>
    Object type_cast() {
        if (id == id_of<T>) return copy();
        std::unordered_set<id_t> vis;
        auto                     res = try_cast(id, id_of<T>, data, vis);
        if (res == nullptr) return {id_of<std::nullptr_t>, nullptr};
        if (!registered.contains(id_of<T>)) {
            if constexpr (requires(T* a) { delete a; }) deleters[id_of<T>] = [](void* v) -> void { delete (T*)v; };
            if constexpr (requires(T a) { new T(a); })
                copiers[id_of<T>] = [](void* v) -> void* { return new T(*((T*)v)); };
            if constexpr (requires(T a, T b) { a == b; })
                comparators[id_of<T>] = [](void* a, void* b) -> bool { return (*((T*)a)) == (*((T*)b)); };
        }
        return {id_of<T>, res};
    }
    operator bool() { return data != nullptr; }
    Object()              = delete;
    Object(const Object&) = delete;
    Object(Object&&)      = delete;
    static inline void register_cast_rule(id_t from, id_t to, void* (*cast)(const void*), CastRuleType type) {
        castrules[{from, to}] = {cast, type};
    }
    static inline void register_predef_rules() {
        register_normal_cast_rule<signed char, bool, CastRuleType::BigToSmall>();
        register_normal_cast_rule<unsigned char, bool, CastRuleType::BigToSmall>();
        register_normal_cast_rule<char, bool, CastRuleType::BigToSmall>();

        register_normal_cast_rule<signed int, bool, CastRuleType::BigToSmall>();
        register_normal_cast_rule<unsigned int, bool, CastRuleType::BigToSmall>();
        register_normal_cast_rule<int, bool, CastRuleType::BigToSmall>();

        register_normal_cast_rule<signed short, bool, CastRuleType::BigToSmall>();
        register_normal_cast_rule<unsigned short, bool, CastRuleType::BigToSmall>();
        register_normal_cast_rule<short, bool, CastRuleType::BigToSmall>();

        register_normal_cast_rule<signed long, bool, CastRuleType::BigToSmall>();
        register_normal_cast_rule<unsigned long, bool, CastRuleType::BigToSmall>();
        register_normal_cast_rule<long, bool, CastRuleType::BigToSmall>();

        register_normal_cast_rule<signed long long, bool, CastRuleType::BigToSmall>();
        register_normal_cast_rule<unsigned long long, bool, CastRuleType::BigToSmall>();
        register_normal_cast_rule<long long, bool, CastRuleType::BigToSmall>();

        register_normal_cast_rule<signed, bool, CastRuleType::BigToSmall>();
        register_normal_cast_rule<unsigned, bool, CastRuleType::BigToSmall>();

        register_normal_cast_rule<const bool, bool, CastRuleType::Equal>();

        register_normal_cast_rule<const char, char, CastRuleType::Equal>();
        register_normal_cast_rule<const signed char, signed char, CastRuleType::Equal>();
        register_normal_cast_rule<const unsigned char, unsigned char, CastRuleType::Equal>();

        register_normal_cast_rule<const int, int, CastRuleType::Equal>();
        register_normal_cast_rule<const signed int, signed int, CastRuleType::Equal>();
        register_normal_cast_rule<const unsigned int, unsigned int, CastRuleType::Equal>();

        register_normal_cast_rule<const short, short, CastRuleType::Equal>();
        register_normal_cast_rule<const signed short, signed short, CastRuleType::Equal>();
        register_normal_cast_rule<const unsigned short, unsigned short, CastRuleType::Equal>();

        register_normal_cast_rule<const long long, long long, CastRuleType::Equal>();
        register_normal_cast_rule<const signed long long, signed long long, CastRuleType::Equal>();
        register_normal_cast_rule<const unsigned long long, unsigned long long, CastRuleType::Equal>();

        register_normal_cast_rule<const long, long, CastRuleType::Equal>();
        register_normal_cast_rule<const signed long, signed long, CastRuleType::Equal>();
        register_normal_cast_rule<const unsigned long, unsigned long, CastRuleType::Equal>();

        register_normal_cast_rule<const float, float, CastRuleType::Equal>();
        register_normal_cast_rule<const double, double, CastRuleType::Equal>();

        register_normal_cast_rule<signed char, unsigned char, CastRuleType::SmallToBig>();
        register_normal_cast_rule<signed int, unsigned int, CastRuleType::SmallToBig>();
        register_normal_cast_rule<signed short, unsigned short, CastRuleType::SmallToBig>();
        register_normal_cast_rule<signed long, unsigned long, CastRuleType::SmallToBig>();
        register_normal_cast_rule<signed long long, unsigned long long, CastRuleType::SmallToBig>();

        register_normal_cast_rule<signed long long, float, CastRuleType::SmallToBig>();
        register_normal_cast_rule<unsigned long long, float, CastRuleType::SmallToBig>();

        register_normal_cast_rule<float, double, CastRuleType::SmallToBig>();

        register_normal_cast_rule<signed char, signed short, CastRuleType::SmallToBig>();
        register_normal_cast_rule<signed short, signed int, CastRuleType::SmallToBig>();
        register_normal_cast_rule<signed int, signed long, CastRuleType::SmallToBig>();
        register_normal_cast_rule<signed long, signed long long, CastRuleType::SmallToBig>();

        register_normal_cast_rule<const char*, std::string, CastRuleType::Equal>();
        register_normal_cast_rule<char*, std::string, CastRuleType::Equal>();

        register_cast_rule(
            id_of<std::string>,
            id_of<const char*>,
            [](const void* from) -> void* {
                auto res = new char*(new char[static_cast<const std::string*>(from)->size() + 1]{0});
                for (std::size_t i = 0; i < static_cast<const std::string*>(from)->size(); i++) {
                    (*res)[i] = (*static_cast<const std::string*>(from))[i];
                }
                return res;
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<std::string>,
            id_of<char*>,
            [](const void* from) -> void* {
                auto res = new char*(new char[static_cast<const std::string*>(from)->size() + 1]{0});
                for (std::size_t i = 0; i < static_cast<const std::string*>(from)->size(); i++) {
                    (*res)[i] = (*static_cast<const std::string*>(from))[i];
                }
                return res;
            },
            CastRuleType::Equal
        );

        copiers[id_of<char*>] = [](void* v) -> void* {
            auto size = std::strlen(*static_cast<char**>(v));
            auto res  = new char*(new char[size + 1]{0});
            for (std::size_t i = 0; i < size; i++) (*res)[i] = (*static_cast<char**>(v))[i];
            return res;
        };
        copiers[id_of<const char*>] = [](void* v) -> void* {
            auto size = std::strlen(*static_cast<char**>(v));
            auto res  = new char*(new char[size + 1]{0});
            for (std::size_t i = 0; i < size; i++) (*res)[i] = (*static_cast<char**>(v))[i];
            return res;
        };

        deleters[id_of<char*>] = [](void* v) -> void {
            auto t = static_cast<char**>(v);
            delete[] *t;
            delete t;
        };
        deleters[id_of<const char*>] = [](void* v) -> void {
            auto t = static_cast<char**>(v);
            delete[] *t;
            delete t;
        };

        register_cast_rule(
            id_of<signed long long>,
            id_of<std::string>,
            [](const void* from) -> void* {
                return new std::string(std::to_string(*static_cast<const signed long long*>(from)));
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<unsigned long long>,
            id_of<std::string>,
            [](const void* from) -> void* {
                return new std::string(std::to_string(*static_cast<const unsigned long long*>(from)));
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<float>,
            id_of<std::string>,
            [](const void* from) -> void* { return new std::string(std::to_string(*static_cast<const float*>(from))); },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<double>,
            id_of<std::string>,
            [](const void* from) -> void* {
                return new std::string(std::to_string(*static_cast<const double*>(from)));
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<std::string>,
            id_of<signed long long>,
            [](const void* from) -> void* {
                return new signed long long(std::stoll(*static_cast<const std::string*>(from)));
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<std::string>,
            id_of<unsigned long long>,
            [](const void* from) -> void* {
                return new unsigned long long(std::stoull(*static_cast<const std::string*>(from)));
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<std::string>,
            id_of<signed int>,
            [](const void* from) -> void* {
                return new signed int(std::stoll(*static_cast<const std::string*>(from)));
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<std::string>,
            id_of<unsigned int>,
            [](const void* from) -> void* {
                return new unsigned int(std::stoull(*static_cast<const std::string*>(from)));
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<std::string>,
            id_of<signed short>,
            [](const void* from) -> void* {
                return new signed short(std::stoll(*static_cast<const std::string*>(from)));
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<std::string>,
            id_of<unsigned short>,
            [](const void* from) -> void* {
                return new unsigned short(std::stoull(*static_cast<const std::string*>(from)));
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<std::string>,
            id_of<signed char>,
            [](const void* from) -> void* {
                return new signed char(std::stoll(*static_cast<const std::string*>(from)));
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<std::string>,
            id_of<unsigned char>,
            [](const void* from) -> void* {
                return new unsigned char(std::stoull(*static_cast<const std::string*>(from)));
            },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<std::string>,
            id_of<float>,
            [](const void* from) -> void* { return new float(std::stof(*static_cast<const std::string*>(from))); },
            CastRuleType::Equal
        );
        register_cast_rule(
            id_of<std::string>,
            id_of<double>,
            [](const void* from) -> void* { return new double(std::stod(*static_cast<const std::string*>(from))); },
            CastRuleType::Equal
        );
    }
    template <typename T>
    static void register_normal_type() {
        if (!registered.contains(id_of<T>)) {
            if constexpr (requires(T* a) { delete a; }) deleters[id_of<T>] = [](void* v) -> void { delete (T*)v; };
            if constexpr (requires(T a) { new T(a); } && !std::is_const_v<T>)
                copiers[id_of<T>] = [](void* v) -> void* { return new T(*((T*)v)); };
            if constexpr (requires(T a, T b) { a == b; })
                comparators[id_of<T>] = [](void* a, void* b) -> bool { return (*((T*)a)) == (*((T*)b)); };
        }
    }
};

struct CallableObject : Object {
private:
    static inline std::unordered_map<id_t, Object (*)(Object&, std::vector<Object*>&)> callableObjects;

    template <typename T, auto I>
    static inline auto cast_helper(std::vector<Object*>& args) {
        auto res =
            args[I]->template type_cast<std::remove_cvref_t<typename CTC::FunctionInfo<T>::args::template type<I>>>();
        if (res)
            return *res.template cast<std::remove_cvref_t<typename CTC::FunctionInfo<T>::args::template type<I>>>();
        throw std::invalid_argument(
            std::
                format("invalid type:at:{},from:{},to:{}.", I, args[I]->get_id(), id_of<std::remove_cvref_t<typename CTC::FunctionInfo<T>::args::template type<I>>>)
        );
    }
    template <typename T, auto... I>
    void register_callable_object_type(std::index_sequence<I...>) {
        callableObjects[get_id()] = [](Object& callable, std::vector<Object*>& args) -> Object {
            if (args.size() != CTC::FunctionInfo<T>::argc) {
                throw std::invalid_argument(
                    std::format("invalid argument count:need:{},given:{}", CTC::FunctionInfo<T>::argc, args.size())
                );
            }
            if constexpr (std::is_same_v<typename CTC::FunctionInfo<T>::ret, void>) {
                ((*callable.cast<T>())(cast_helper<T, I>(args)), ...);
                return {nullptr};
            } else {
                return {((*callable.cast<T>())(cast_helper<T, I>(args)), ...)};
            }
        };
    }

public:
    bool   is_callable() { return callableObjects.contains(get_id()); }
    Object call(std::vector<Object*>& args) {
        if (is_callable()) return callableObjects[get_id()](*this, args);
        return {nullptr};
    }
    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, CallableObject> && !std::is_same_v<std::remove_cvref_t<T>, Object>)
    CallableObject(const T& data) : Object(data) {
        register_callable_object_type<T>(std::make_index_sequence<CTC::FunctionInfo<T>::argc>{});
    }
    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, CallableObject> && !std::is_same_v<std::remove_cvref_t<T>, Object>)
    CallableObject(T&& data) : Object(std::move(data)) {
        register_callable_object_type<T>(std::make_index_sequence<CTC::FunctionInfo<T>::argc>{});
    }
    static inline std::shared_ptr<std::vector<Object*>> make_args(const auto&... args) {
        std::vector<Object*> targs;
        (targs.push_back(new Object(args)), ...);
        return std::make_shared<std::vector<Object*>>(std::move(targs));
    }
};
} // namespace AnyObject
