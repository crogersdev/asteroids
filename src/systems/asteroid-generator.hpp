#pragma once

#include "../components.hpp"
#include "../constants.hpp"
#include "../entities.hpp"
#include "../helpers/asteroid-info.hpp"
#include "../helpers/math-stuff.hpp"

#include <vector>
#include <raylib.h>

namespace crogersdev {

inline std::vector<Line> generate_asteroid_shape(asteroid_size_t size, uint32_t radius, Color color, float thickness) {
    std::discrete_distribution<uint32_t> radius_tolerance({ 65, 20, 10, 5 });
    uint32_t sides = my_rng(11, 23, Dist::Uniform);
    uint32_t sz = static_cast<uint32_t>(size);
    float actual_radius = radius * sz;

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
        actual_radius = radius * sz;
    }
    std::vector<Line> asteroid_shape;
    asteroid_shape.push_back(Line{ asteroid_coords.at(0), asteroid_coords.at(1), color, thickness });

    for (int ac = 1; ac < sides-1; ac++) {
        asteroid_shape.push_back(Line{ asteroid_coords.at(ac), asteroid_coords.at(ac+1), color, thickness });
    }

    asteroid_shape.push_back(Line{ asteroid_coords.at(sides-1), asteroid_coords.at(0), color, thickness });

    return asteroid_shape;
}

inline void spawn_asteroid(Registry& r, Entity id, uint32_t radius, asteroid_size_t sz, Vector2 pos, Vector2 vel, float theta) {
    r.add(id, Size{ radius, sz });
    r.add(id, Transform{ pos, vel, theta, 0.f, 1.f, 0.f });
    r.add(id, AsteroidShape{ generate_asteroid_shape(sz, radius, RED, 1.25f) });
}

inline void spawn_asteroids_for_level(Registry& r, GameState gs, uint32_t total) {
    const Vector2 one_third  = { SCREEN_WIDTH / 3.f, SCREEN_HEIGHT / 3.f };
    const Vector2 two_thirds = { 2.f*SCREEN_WIDTH / 3.f, 2.f*SCREEN_HEIGHT / 3.f };

    for (uint8_t i = 0; i < total; ++i) {
        Vector2 pos{};
        pos.x = my_rng(0.f, two_thirds.x, Dist::Uniform);
        if (pos.x > one_third.x) pos.x += one_third.x;

        pos.y = my_rng(0.f, two_thirds.y, Dist::Uniform);
        if (pos.y > one_third.y) pos.y += one_third.y;

        float theta = my_rng(0.f, 2.f * PI, Dist::Uniform);
        Vector2 vel = { cosf(theta) * asteroid_init_speed, sinf(theta) * asteroid_init_speed };
        spawn_asteroid(r, r.create(), asteroid_radius, asteroid_size_t::LARGE, pos, vel, theta);

        gs.remaining_asteroids++;
    }
}

} // end namespace
