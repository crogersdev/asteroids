#pragma once

#include "../components.hpp"
#include "../entities.hpp"

#include <cmath>
#include <random>

#include <raylib.h>

namespace crogersdev {


inline void game_init(Registry& registry) {
    float x = GetScreenWidth();
    float y = GetScreenHeight();
    Vector2 center = { x / 2.f, y / 2.f };

    float    player_turn_speed = .08f;
    float    player_max_speed = 225.f;
    float    player_acceleration = 500.f;
    float    player_drag_coeff = .99;
    uint32_t player_max_ammo = 6;

    Entity player = registry.create();
    registry.add(player, PlayerInput{ false, false, false, false });
    registry.add(player, Transform{
        { center.x, center.y },
        { 0.f, 0.f },
        player_turn_speed,
        player_drag_coeff });
    registry.add(player, Weapon{ 
        player_max_ammo,
        0.f,
        0.f });
    registry.add(player, PolygonShip{{
        Line{{ -10.f,  +4.f }, {   0.f, -14.f }, RED, 1.5f },
        Line{{   0.f, -14.f }, { +10.f,  +4.f }, BLUE, 1.5f },
        Line{{ +10.f,  +4.f }, {   0.f,   0.f }, GREEN, 1.5f },
        Line{{   0.f,   0.f }, { -10.f,  +4.f }, YELLOW, 1.5f }}, 
        Vector2{ 0.f, -1.f },
        player_max_speed,
        player_acceleration });

    constexpr uint32_t INITIAL_ASTEROIDS = 5;
    std::array<Entity, INITIAL_ASTEROIDS> asteroids;

    std::random_device rd;
    std::mt19937 gen(rd());
    Vector2 one_third = { x / 3.f, y / 3.f };
    Vector2 two_thirds = { 2.f*x / 3.f, 2.f*y / 3.f };
    std::uniform_int_distribution<uint32_t> dist_x(0, two_thirds.x);
    std::uniform_int_distribution<uint32_t> dist_y(0, two_thirds.y);
    std::uniform_real_distribution<float> dist_theta(0, 2*PI);

    float    asteroid_speed = 100.f;
    uint32_t asteroid_size  = 4;

    for (uint8_t i = 0; i < INITIAL_ASTEROIDS; ++i) {
        float a_x = dist_x(gen);
        if (a_x > one_third.x) a_x += one_third.x;

        float a_y = dist_y(gen);
        if (a_y > one_third.y) a_y += one_third.y;

        asteroids[i] = registry.create();
        float theta = dist_theta(gen);
        float dir_x = cos(theta);
        float dir_y = sin(theta);
        registry.add(asteroids[i], Size{ asteroid_size });
        registry.add(asteroids[i], Transform{ { a_x, a_y }, { dir_x * asteroid_speed, dir_y * asteroid_speed }, 0.f });
        registry.add(asteroids[i], Asteroid{{
            Line{{ -40.f, +40.f }, { -40.f, -40.f }, RED, 1.25f },
            Line{{ -40.f, -40.f }, { +40.f, -40.f }, RED, 1.25f },
            Line{{ +40.f, -40.f }, { +40.f, +40.f }, RED, 1.25f },
            Line{{ +40.f, +40.f }, { -40.f, +40.f }, RED, 1.25f }
        }});
    }
}

} // end namespace
