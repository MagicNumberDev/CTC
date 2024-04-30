#pragma once
#include <concepts>
#include <filesystem>


extern "C" std::size_t std::strlen(const char*);
extern "C" int         std::strcmp(const char*, const char*);

#define EXPECTION(COND, ...)                                                                                           \
    [&] {                                                                                                              \
        if (COND) try {                                                                                                \
                return Expection{__VA_ARGS__};                                                                         \
            } catch (...) {}                                                                                           \
        return Expection<decltype(__VA_ARGS__)>{};                                                                     \
    }()

namespace opt_helper {

template <typename T>
struct Expection {
    T value;
    constexpr Expection()                            = default;
    constexpr Expection(const Expection&)            = default;
    constexpr Expection(Expection&&)                 = default;
    constexpr Expection& operator=(const Expection&) = default;
    constexpr Expection& operator=(Expection&&)      = default;
    constexpr Expection(const T& d) { value = d; }
    constexpr T expect(auto&& f) {
        if constexpr (std::convertible_to<decltype(f()), T>) {
            return f();
        } else f();
        return value;
    }
};


template <typename T>
struct OptVal {
    T    data;
    auto toi() { return EXPECTION(data != nullptr, std::stoi(data)); }
    auto tol() { return EXPECTION(data != nullptr, std::stol(data)); }
    auto toll() { return EXPECTION(data != nullptr, std::stoll(data)); }
    auto toul() { return EXPECTION(data != nullptr, std::stoul(data)); }
    auto toull() { return EXPECTION(data != nullptr, std::stoull(data)); }
    auto tod() { return EXPECTION(data != nullptr, std::stod(data)); }
    auto tof() { return EXPECTION(data != nullptr, std::stof(data)); }
    auto told() { return EXPECTION(data != nullptr, std::stold(data)); }
    auto tob() { return EXPECTION(data != nullptr, std::strcmp(data, "true") == 0); }
    auto top() { return EXPECTION(data != nullptr, std::filesystem::path(data)); }
    auto empty() { return data == nullptr || data[0] == '\0'; }
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