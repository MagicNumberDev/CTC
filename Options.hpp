#pragma once
#include <cstdlib>
#include <filesystem>
#include <string>


extern "C" std::size_t strlen(const char*);
namespace opt_helper {
template <typename T>
struct OptVal {
    T    data;
    auto toi() { return std::stoi(data); }
    auto tol() { return std::stol(data); }
    auto toll() { return std::stoll(data); }
    auto toul() { return std::stoul(data); }
    auto toull() { return std::stoull(data); }
    auto tod() { return std::stod(data); }
    auto tof() { return std::stof(data); }
    auto told() { return std::stold(data); }
    auto tob() { return strcmp(data, "true") == 0; }
    auto top() { return std::filesystem::path(data); }
};

template <typename T, typename U>
struct Parser {
    T argc;
    U argv;


    template <auto first_opt, auto... opt>
    OptVal<const char*> get() {
        const char* r = nullptr;
        for (T i = 0; i < argc; i++) {
            bool not_match = false;
            T    index     = 0;
            for (auto c : first_opt) {
                if (argv[i][index] != c || argv[i][index] == '\0') {
                    not_match = true;
                    break;
                }
                index++;
            }
            if (not_match) continue;
            if (argv[i][index] != '=' || argv[i][index] == '\0') r = nullptr;
            else r = &argv[i][(argv[i][index] == '=' ? index + 1 : index)];
        }
        if (r == nullptr) {
            if constexpr (sizeof...(opt) != 0) {
                return get<opt...>();
            } else {
                return {nullptr};
            }
        } else {
            return {r};
        }
    }
};
} // namespace opt_helper