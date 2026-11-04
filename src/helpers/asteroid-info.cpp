#include "asteroid-info.hpp"

#include <algorithm>

namespace crogersdev {

std::ostream& operator<<(std::ostream& os, const asteroid_size_t& size) {
    switch(size) {
        case asteroid_size_t::LARGE:  return os << "LARGE";
        case asteroid_size_t::MEDIUM: return os << "MEDIUM";
        case asteroid_size_t::SMALL:  return os << "SMALL";
        case asteroid_size_t::TINY:   return os << "TINY";
        default: break;
    }
}

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