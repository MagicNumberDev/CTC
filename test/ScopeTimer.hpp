#pragma once
#include <chrono>
#include <print>
namespace testing {
struct ScopeTimer {
    std::chrono::high_resolution_clock                 clock;
    std::chrono::time_point<std::chrono::steady_clock> begin;
    const char*                                        name;
    ScopeTimer(const char* name) : name(name), begin(clock.now()) {}
    ~ScopeTimer() {
        auto end      = clock.now();
        auto duration = end - begin;
        std::println("name:{},duration:{}", name, duration);
    }
};
} // namespace testing
