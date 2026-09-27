#pragma once

namespace crogersdev {

enum class asteroid_size_t {
    TINY   = 1,
    SMALL  = 2,
    MEDIUM = 3,
    LARGE  = 4,
    COUNT  = LARGE,
    ZERO   = TINY
};

asteroid_size_t operator+(asteroid_size_t, int);
asteroid_size_t operator+(int, asteroid_size_t);
asteroid_size_t operator-(asteroid_size_t, int);
asteroid_size_t operator-(int, asteroid_size_t);

}; // end namespace