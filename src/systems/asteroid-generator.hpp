#pragma once

#include "../components.hpp"
#include "../helpers.hpp"

#include <vector>

#include <raylib.h>

namespace crogersdev {

inline std::vector<Line> generate_asteroid(uint32_t size, uint32_t radius, Color color, float thickness) {
    std::discrete_distribution<uint32_t> radius_tolerance({ 65, 20, 10, 5 });
    uint32_t sides = my_rng(11, 23, Dist::Uniform);
    uint32_t actual_radius = radius * size;

    float theta = 0.f;
    uint32_t radius_wobble = 0;
    std::vector<Vector2> asteroid_coords;
    for (int side = 0; side < sides; side++) {
        radius_wobble = radius_tolerance(get_generator());
        if (radius_wobble == 1) { actual_radius += actual_radius * .1f; }
        if (radius_wobble == 2) { actual_radius -= actual_radius * .2f; }
        if (radius_wobble == 3) { actual_radius -= actual_radius * .3f; }
        theta += (2.f*PI / sides);
        asteroid_coords.push_back({ cos(theta) * actual_radius, sin(theta) * actual_radius });
        actual_radius = size * radius;
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
