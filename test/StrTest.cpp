#include "../CTCStr.hpp"
#include "../CTStr.hpp"
#include "../CTVec.hpp"
#include <cstdio>
int main() {
  CTC::CTCStr str{"123"};
  printf("%s", str.substr<3>(0).c_str());
  CTC::CTStr str2{"234"};
  printf("\n%s", str2.substr<3>(0).as_zero_end_str().data);
  CTC::CTVec vec{'1', '2', '3', '\0'};
  printf("\n%s", vec.subvec<2>(1).data);
}