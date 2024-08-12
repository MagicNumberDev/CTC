
#include "../CTCompare.hpp"

#include "../CTStr.hpp"
bool test_CTSTR() {
    CTC::CTStr a{"123"};
    CTC::CTStr b{"123"};
    CTC::CTStr c{"1234"};
    return a == b && b != c;
}
#include "../CTCStr.hpp"
bool test_CTCSTR() {
    CTC::CTCStr a{"123"};
    CTC::CTCStr b{"123"};
    CTC::CTCStr c{"1234"};
    return a == b && b != c;
}
bool test_STR() {
    CTC::CTCStr a{"123"};
    CTC::CTStr  b{"123"};
    CTC::CTCStr c{"1234"};
    return a == b && b != c;
}
#include "../CTVec.hpp"
bool test_CTVEC() {
    CTC::CTVec a{1, 2, 3};
    CTC::CTVec b{1, 2, 3};
    CTC::CTVec c{1, 2, 3, 4};
    return a == b && b != c;
}
#include "../CTDArray.hpp"
consteval bool test_CTDARRAY() {
    CTC::CTDArray a{1};
    a.push_back(1);
    a.pop_back();
    return a.size() == 1;
}
#include "../Type.hpp"
bool test_TYPE() { return CTC::name_of<int>() == CTC::CTCStr{"int"} && CTC::hash_of<int>() != CTC::hash_of<double>(); }
#include "../Any.hpp"
consteval bool test_ANY() {
    CTC::Any a(0);
    return a.cast_to_ref<int>() == 0;
}
int main() {
    return test_CTSTR() && test_CTCSTR() && test_STR() && test_CTVEC() && test_CTDARRAY() && test_TYPE() && test_ANY()
             ? 0
             : 1;
}