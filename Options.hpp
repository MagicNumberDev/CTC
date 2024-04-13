#pragma once
#include <cstdlib>
extern "C" std::size_t strlen(const char*);
namespace opt_helper {

template <typename T, typename U>
struct Parser {
    T argc;
    U argv;
    template <auto first_opt, auto... opt>
    const char* get() {
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
                return nullptr;
            }
        } else {
            return r;
        }
    }
};
} // namespace opt_helper