#include "../Object.hpp"
#include <cstdio>
#include <iostream>
#if __has_include(<stacktrace>)
#include <stacktrace>
#endif
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
auto& test(auto v) {
    printf("%p\n", v.template cast<X>());
    return *(v.template cast<X>());
}
#include <print>


int main() {
    // testing::ScopeTimer mainTimer{"main"};
    X x{};
    printf("%p\n", std::addressof(x));
    AnyObject::Object obj(std::move(x));
    puts("=====");
    auto b = obj.ref();
    auto c = obj.ref();
    auto t = obj.ref();
    auto d = b.copy();
    // printf("%d\n", AnyObject::Object::refcounts[t.data]);
    printf("%p,%p,%p\n", b.cast<X>(), c.cast<X>(), t.cast<X>());
    printf("%p\n", std::addressof(test(b.ref())));
    printf("end\n");
    AnyObject::Object::register_pre_def_rules();
    AnyObject::Object e{std::string("1000")};
    auto              r = e.type_cast<char*>();
    std::println("{}", *r.cast<char*>());
    std::vector<AnyObject::Object> a;
    AnyObject::Object              test1(1);
    a.push_back(std::move(test1));
    AnyObject::CallableObject callableObject([](const int& a) {
        printf("%d\n", a);
        return std::string("123");
    });
    auto                      res = callableObject.call(a);
    printf("%s\n", res.cast<std::string>()->c_str());
    callableObject.call(*AnyObject::CallableObject::make_args(1));
    std::string                    s{};
    std::vector<AnyObject::Object> args;
    while (std::cin >> s) {
        args.emplace_back(s);
    }
    callableObject.call(args);
    AnyObject::Object n1;
    n1 = AnyObject::Object{X{}};
}
