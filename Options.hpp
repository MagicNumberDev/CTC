#pragma once
#include <concepts>
#include <cstring>
#include <exception>
#include <filesystem>
#include <string>


#define EXPECTION(COND, VALUE)                                                                                         \
    [&] {                                                                                                              \
        if (COND) try {                                                                                                \
                return ::opt_helper::Expection<decltype(VALUE)>{VALUE};                                                \
            } catch (...) {                                                                                            \
                return ::opt_helper::Expection<decltype(VALUE)>{::opt_helper::FailType::Try};                          \
            }                                                                                                          \
        else return ::opt_helper::Expection<decltype(VALUE)>{::opt_helper::FailType::If};                              \
    }()

namespace opt_helper {
enum class FailType { If, Try, Non };
template <typename T>
struct Expection {
    T        value;
    FailType reason                                  = FailType::Non;
    constexpr Expection()                            = default;
    constexpr Expection(const Expection&)            = default;
    constexpr Expection(Expection&&)                 = default;
    constexpr Expection& operator=(const Expection&) = default;
    constexpr Expection& operator=(Expection&&)      = default;
    constexpr Expection(const T& d) {
        value  = d;
        reason = FailType::Non;
    }
    constexpr Expection(const FailType& f) { reason = f; }
    constexpr T& expect(auto&& f) {
        if (reason != FailType::Non) {
            if constexpr (std::convertible_to<decltype(f(reason)), T>) {
                value = f(reason);
            } else {
                f(reason);
                std::terminate();
            }
        }
        return value;
    }
};


template <typename T>
struct OptVal {
    const T* data;
    auto     tos() {
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, char>) {
            return EXPECTION(data != nullptr, std::string(data));
        } else {
            return EXPECTION(data != nullptr, std::wstring(data));
        }
    }
    auto toi() { return EXPECTION(data != nullptr, std::stoi(data)); }
    auto tol() { return EXPECTION(data != nullptr, std::stol(data)); }
    auto toll() { return EXPECTION(data != nullptr, std::stoll(data)); }
    auto toul() { return EXPECTION(data != nullptr, std::stoul(data)); }
    auto toull() { return EXPECTION(data != nullptr, std::stoull(data)); }
    auto tod() { return EXPECTION(data != nullptr, std::stod(data)); }
    auto tof() { return EXPECTION(data != nullptr, std::stof(data)); }
    auto told() { return EXPECTION(data != nullptr, std::stold(data)); }
    auto tob() {
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, char>) {
            return EXPECTION(data != nullptr, std::strcmp(data, "true") == 0);
        } else {
            return EXPECTION(data != nullptr, std::wcscmp(data, "true") == 0);
        }
    }
    auto top() { return EXPECTION(data != nullptr, std::filesystem::path(data)); }
    auto empty() { return data == nullptr || data[0] == '\0'; }
};

struct ExistResult {
    bool exist;
    bool has_arg;
    constexpr operator bool() { return exist; }
};

template <typename T, typename U>
struct Parser {
    T   argc;
    U** argv;
    template <auto first_opt, auto... opt>
    OptVal<U> get() {
        if constexpr (std::is_same_v<std::remove_cvref_t<U>, char>) {
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
                if (argv[i][index] != '=' || argv[i][index] == '\0'
                    || (argv[i][index] == '=' && argv[i][index + 1] == '\0'))
                    r = nullptr;
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
        } else {
            const wchar_t* r = nullptr;
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
                if (argv[i][index] != '=' || argv[i][index] == '\0'
                    || (argv[i][index] == '=' && argv[i][index + 1] == '\0'))
                    r = nullptr;
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
    }
    template <auto first_opt, auto... opt>
    ExistResult exist() {
        if constexpr (std::is_same_v<std::remove_cvref_t<U>, char>) {
            for (T i = 0; i < argc; i++) {
                if (std::strlen(argv[i]) < first_opt.length) continue;
                bool match = true;
                for (::std::size_t j = 0; j < std::strlen(argv[i]); j++) {
                    if (argv[i][j] != first_opt[j]) {
                        match = false;
                        break;
                    }
                }
                if (std::strlen(argv[i]) > first_opt.length && argv[i][std::strlen(argv[i]) + 1] != '=') match = false;
                if (match)
                    return {true, std::strlen(argv[i]) > first_opt.length && argv[i][std::strlen(argv[i]) + 1] == '='};
            }
            if constexpr (sizeof...(opt) != 0) return exist<opt...>();
            else return {false, false};
        } else {
            for (T i = 0; i < argc; i++) {
                if (std::wcslen(argv[i]) < first_opt.length) continue;
                bool match = true;
                for (::std::size_t j = 0; j < std::wcslen(argv[i]); j++) {
                    if (argv[i][j] != first_opt[j]) {
                        match = false;
                        break;
                    }
                }
                if (std::wcslen(argv[i]) > first_opt.length && argv[i][std::wcslen(argv[i]) + 1] != '=') match = false;
                if (match)
                    return {true, std::wcslen(argv[i]) > first_opt.length && argv[i][std::wcslen(argv[i]) + 1] == '='};
            }
            if constexpr (sizeof...(opt) != 0) return exist<opt...>();
            else return {false, false};
        }
    }
};

} // namespace opt_helper
