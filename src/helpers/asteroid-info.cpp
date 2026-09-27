#include "asteroid-info.hpp"

#include <algorithm>

namespace crogersdev {

asteroid_size_t operator+(asteroid_size_t s, int steps) {
    int current = static_cast<int>(s);
    int max     = static_cast<int>(asteroid_size_t::COUNT);
    int min     = static_cast<int>(asteroid_size_t::ZERO);
    int next    = std::clamp(current + steps, min, max - min);
    return static_cast<asteroid_size_t>(next);
}

asteroid_size_t operator+(int steps, asteroid_size_t s) {
    return s + steps;
}

asteroid_size_t operator-(asteroid_size_t s, int steps) {
    int current = static_cast<int>(s);
    int max     = static_cast<int>(asteroid_size_t::COUNT);
    int min     = static_cast<int>(asteroid_size_t::ZERO);
    int next    = std::clamp(current - steps, min, max - min);
    return static_cast<asteroid_size_t>(next);
}

asteroid_size_t operator-(int steps, asteroid_size_t s) {
    return s - steps;
}

} // end namespace