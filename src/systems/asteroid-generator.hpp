#pragma once

#include "../components.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include <raylib.h>

namespace crogersdev {

inline std::vector<Line> generate_asteroid(uint32_t size, Color color, float thickness) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<uint32_t> sides_generator(11, 23);
    std::discrete_distribution<uint32_t> radius_tolerance({ 65, 20, 10, 5 });
    uint32_t sides = sides_generator(gen);
    uint32_t radius = size * 12;

    float theta = 0.f;
    uint32_t radius_wobble = 0;
    std::vector<Vector2> asteroid_coords;
    for (int side = 0; side < sides; side++) {
        radius_wobble = radius_tolerance(gen);
        if (radius_wobble == 1) { radius += radius * .15; }
        if (radius_wobble == 2) { radius -= radius * .2; }
        if (radius_wobble == 3) { radius -= radius * .5; }
        theta += (2.f*PI / sides);
        asteroid_coords.push_back({ cos(theta) * radius, sin(theta) * radius });
        radius = size * 10;
    }
    std::vector<Line> asteroid_shape;
    asteroid_shape.push_back(Line{ asteroid_coords.at(0), asteroid_coords.at(1), color, thickness });

    for (int ac = 1; ac < sides-1; ac++) {
        asteroid_shape.push_back(Line{ asteroid_coords.at(ac), asteroid_coords.at(ac+1), color, thickness });
    }

    asteroid_shape.push_back(Line{ asteroid_coords.at(sides-1), asteroid_coords.at(0), color, thickness });

    return asteroid_shape;
}

}
