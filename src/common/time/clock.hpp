#pragma once

#include <chrono>

class Clock {
    std::chrono::time_point<std::chrono::steady_clock> start
    = std::chrono::steady_clock::now();

public:
    std::chrono::duration<float> restart() {
        auto end = std::chrono::steady_clock::now();
        auto s = std::chrono::duration<float>(end - start);
        start = end;
        return s;
    }
};
