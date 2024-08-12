#include "../Function.hpp"
#include "../CTCStr.hpp"

template <typename T> consteval auto get_type_name() {
  return CTC::CTCStr{__PRETTY_FUNCTION__};
}

int main(int, char **) {
  CTC::Function a([] {});
  CTC::Function b(main);
  b(1, {});
}