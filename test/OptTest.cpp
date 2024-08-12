#include "../CTStr.hpp"
#include "../Options.hpp"

int main(int argc, char** argv) {
    opt_helper::Parser parser{argc, argv};
    auto               t = CTC::CTStr{L"--test"};
    parser.exist<CTC::CTStr{L"--test"}>() ? puts("true") : puts("false");
    puts(parser.get<CTC::CTStr{L"--test2"}>().tos().expect([](auto) { return ""; }).c_str());
}