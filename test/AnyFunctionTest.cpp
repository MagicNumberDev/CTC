#include "../AnyFunction.hpp"
#if __has_include(<stacktrace>)
#include <stacktrace>
#endif
#include <cstdio>
struct X {
    X() {
#ifdef _MSC_VER
        puts(__FUNCSIG__);
        puts(std::to_string(std::stacktrace::current()).c_str());
#else
        puts(__PRETTY_FUNCTION__);
#endif
    }
    X(const X&) {
#ifdef _MSC_VER
        puts(__FUNCSIG__);
        puts(std::to_string(std::stacktrace::current()).c_str());
#else
        puts(__PRETTY_FUNCTION__);
#endif
    }
    X(X&&) {
#ifdef _MSC_VER
        puts(__FUNCSIG__);
        puts(std::to_string(std::stacktrace::current()).c_str());
#else
        puts(__PRETTY_FUNCTION__);
#endif
    }
    ~X() {
#ifdef _MSC_VER
        puts(__FUNCSIG__);
        puts(std::to_string(std::stacktrace::current()).c_str());
#else
        puts(__PRETTY_FUNCTION__);
#endif
    }
};
consteval auto test1() {
    CTC::AnyFunction f{[](int a, CTC::Any b) -> CTC::Any {
        switch (b.type_hash) {
        case CTC::hash_of<int>():
            return a + b.cast_to_ref<int>();
        case CTC::hash_of<double>():
            return a + b.cast_to_ref<double>();
        default:
            return a;
        }
    }};
    return f({0, 10}).cast_to_ref<int>() + f({0, 10.0}).cast_to_ref<double>();
}
int main() {
    printf("%f\n", test1());
    CTC::AnyFunction f{[](int a, CTC::Any b) -> CTC::Any {
        switch (b.type_hash) {
        case CTC::hash_of<int>():
            return a + b.cast_to_ref<int>();
        case CTC::hash_of<double>():
            return a + b.cast_to_ref<double>();
        default:
            return a;
        }
    }};
    for (auto h : f.argTypesHash) {
        printf("%llu\n", h);
    }
    printf("====\n");
    for (auto h : CTC::details::do_arg_types_hash<decltype([](int a, CTC::Any b) {})>(
             std::make_index_sequence<CTC::FunctionInfo<decltype([](int a, CTC::Any b) {})>::argc>{}
         ))
        printf("%llu\n", h);
    // printf("%llu\n%llu\n", CTC::hash_of<int>(), CTC::hash_of<CTC::Any>());
}