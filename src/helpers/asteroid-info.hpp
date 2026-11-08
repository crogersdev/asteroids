#pragma once

#include <algorithm>
#include <array>
#include <iostream>

namespace crogersdev {

enum class asteroid_size_t {
    TINY   = 1,
    SMALL  = 2,
    MEDIUM = 3,
    LARGE  = 4,
    COUNT  = LARGE,
    ZERO   = TINY
};

inline constexpr std::array all_asteroid_sizes{
    asteroid_size_t::TINY,
    asteroid_size_t::SMALL,
    asteroid_size_t::MEDIUM,
    asteroid_size_t::LARGE,
};

static_assert(
    all_asteroid_sizes.size() == static_cast<int>(asteroid_size_t::LARGE) - static_cast<int>(asteroid_size_t::TINY) + 1,
    "You broke the asteroid_size_t enum somewhere.");

inline constexpr const char* to_c_str(asteroid_size_t size) {
    switch(size) {
        case asteroid_size_t::LARGE:  return "LARGE";
        case asteroid_size_t::MEDIUM: return "MEDIUM";
        case asteroid_size_t::SMALL:  return "SMALL";
        case asteroid_size_t::TINY:   return "TINY";
        // NOTE: explicitly removing "default" to cause warnings
        //       issued by compiler when we don't cover all
        //       cases and enum values.  continued below...
    }

    // ... but having a return statement down here still covers integer
    // values that our enum can accept (like static_cast<asteroid_size_t<88>)
    // which compiles but can't convert to str
    return "UNDEFINED";
}

inline std::ostream& operator<<(std::ostream& os, asteroid_size_t size) {
    return os << to_c_str(size); 
}

inline asteroid_size_t operator+(asteroid_size_t s, int steps) {
    int current = static_cast<int>(s);
    int max     = static_cast<int>(asteroid_size_t::COUNT);
    int min     = static_cast<int>(asteroid_size_t::ZERO);
    int next    = std::clamp(current + steps, min, max);
    return static_cast<asteroid_size_t>(next);
}

inline asteroid_size_t operator+(int steps, asteroid_size_t s) {
    return s + steps;
}

inline asteroid_size_t operator-(asteroid_size_t s, int steps) {
    int current = static_cast<int>(s);
    int max     = static_cast<int>(asteroid_size_t::COUNT);
    int min     = static_cast<int>(asteroid_size_t::ZERO);
    int next    = std::clamp(current - steps, min, max);
    return static_cast<asteroid_size_t>(next);
}

inline asteroid_size_t operator-(int steps, asteroid_size_t s) {
    return s - steps;
}

}; // end namespace