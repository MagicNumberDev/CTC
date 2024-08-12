#include "../AnyRef.hpp"
#include <cstdio>
#include <iostream>
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
    // X(const X&) = delete;
    ~X() {
#ifdef _MSC_VER
        puts(__FUNCSIG__);
        puts(std::to_string(std::stacktrace::current()).c_str());
#else
        puts(__PRETTY_FUNCTION__);
#endif
    }
};
int main() {
    CTC::ConstAnyRef ref(X{});
    auto             t = ref;
    std::cout << ref.data << " " << t.data;
}