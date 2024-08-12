#include "../Any.hpp"
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
consteval void test() {
  CTC::Any a = 1;
  CTC::Any b = a;
}
int main() {
  CTC::Any t([]() {});
  auto b = t;
  test();
  CTC::Any c{1}, d{1};
  X e;
  CTC::Any f{e};
  b.cast_to_pointer<int>();
}