#pragma once
#include "Any.hpp"
#include "CTDArray.hpp"
#include "Type.hpp"

namespace CTC {
namespace details {
template <auto I, auto Argc>
constexpr auto any_call_base(auto& fn, CTDArray<Any>& args, auto&... pack) -> Any {
    if constexpr (I < Argc) {
        if constexpr (sizeof...(pack) == 0) {
            return any_call_base<I + 1, Argc>(fn, args, args[I]);
        } else {
            return any_call_base<I + 1, Argc>(fn, args, pack..., args[I]);
        }
    } else {
        if constexpr (sizeof...(pack) > 0) {
            if constexpr (std::is_same_v<void, decltype(fn(pack...))>) {
                fn(pack...);
                return {};
            } else {
                return {fn(pack...)};
            }
        } else {
            if constexpr (std::is_same_v<void, decltype(fn())>) {
                fn();
                return {};
            } else {
                return {fn()};
            }
        }
    }
}
template <typename T>
constexpr auto any_call(T& fn, CTDArray<Any>& args) {
    return any_call_base<0, FunctionInfo<T>::argc>(fn, args);
}
template <typename T, auto... I>
constexpr auto do_arg_types_hash(std::index_sequence<I...> indexs) {
    CTDArray<std::size_t> res;
    (res.push_back(hash_of<typename FunctionInfo<T>::args::template type<I>>()), ...);
    return res;
}
} // namespace details
struct AnyFunction {
    Any fn;
    Any (*call)(Any&, CTDArray<Any>&);
    CTDArray<std::size_t> argTypesHash;
    constexpr AnyFunction() : fn(), call(nullptr) {}
    template <typename T>
    constexpr AnyFunction(const T& fn)
    : fn(fn),
      call([](Any& fn, CTDArray<Any>& args) -> Any { return details::any_call(fn.cast_to_ref<T>(), args); }),
      argTypesHash(details::do_arg_types_hash<T>(std::make_index_sequence<CTC::FunctionInfo<T>::argc>{})) {}
    constexpr Any operator()(CTDArray<Any>& args) {
        if (check(args)) return call(fn, args);
        std::unreachable();
    }
    constexpr Any operator()(const CTDArray<Any>& args) {
        auto targs = args;
        if (check(targs)) return call(fn, targs);
        std::unreachable();
    }

private:
    constexpr bool check(CTDArray<Any>& args) {
        if (args.size() != argTypesHash.size()) {
            return false;
        }
        for (std::size_t i = 0; i < args.size(); i++) {
            if (args[i].type_hash != argTypesHash[i] && argTypesHash[i] != hash_of<Any>()) {
                return false;
            }
        }
        return true;
    }
};
} // namespace CTC
