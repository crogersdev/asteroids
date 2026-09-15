#pragma once

#include <algorithm>

namespace crogersdev {

enum class asteroid_size_t {
    TINY   = 1,
    SMALL  = 2,
    MEDIUM = 3,
    LARGE  = 4,
    COUNT  = 4
};

asteroid_size_t operator+(asteroid_size_t s, int steps) {
    int current = static_cast<int>(s);
    int total = static_cast<int>(asteroid_size_t::COUNT);
    int next = std::clamp(current + steps, 0, total - 1);
    return static_cast<asteroid_size_t>(next);
}

asteroid_size_t operator+(int steps, asteroid_size_t s) {
    return s + steps;
}

asteroid_size_t operator-(asteroid_size_t s, int steps) {
    int current = static_cast<int>(s);
    int total   = static_cast<int>(asteroid_size_t::COUNT);
    int next    = std::clamp(current - steps, 0, total - 1);
    return static_cast<asteroid_size_t>(next);
}

asteroid_size_t operator-(int steps, asteroid_size_t s) {
    return s - steps;
}

} // end namespace