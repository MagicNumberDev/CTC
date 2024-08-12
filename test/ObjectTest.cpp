#include "../Object.hpp"
#include <cstdio>
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
  X(const X &) {
#ifdef _MSC_VER
    puts(__FUNCSIG__);
    puts(std::to_string(std::stacktrace::current()).c_str());
#else
    puts(__PRETTY_FUNCTION__);
#endif
  }
  X(X &&) {
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
auto &test(auto v) {
  printf("%p\n", v.template cast<X>());
  return *(v.template cast<X>());
}
int main() {
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
}