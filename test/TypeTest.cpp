#include "../Type.hpp"
#include <cstdio>
int main() {
  puts(CTC::name_of<1 * 1>());
  puts(CTC::name_of<int>());
  puts(CTC::name_of<CTC::TypeContainer<int, bool, long>::type<2>>());
  puts(CTC::name_of<CTC::FunctionInfo<decltype([](int) {})>::argc>());
  puts(CTC::name_of<CTC::FunctionInfo<decltype(main)>::argc>());
  puts(CTC::name_of<CTC::FunctionInfo<decltype(puts)>::argc>());
}