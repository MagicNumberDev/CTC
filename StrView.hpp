#include <cstddef>
namespace CTC {
template <typename T>
struct StrView {
    T*          data;
    std::size_t length;
};
} // namespace CTC