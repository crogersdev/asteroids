#pragma once

#include "asteroid-generator.hpp"
#include "../constants.hpp"
#include "../components.hpp"
#include "../entities.hpp"
#include "../helpers.hpp"

#include <cmath>

#include <raylib.h>

namespace crogersdev {

inline void game_init(Registry& registry) {
    const float game_screen_x = GetScreenWidth();
    const float game_screen_y = GetScreenHeight();
    const Vector2 game_screen = Vector2{ game_screen_x, game_screen_y };
    const Vector2 game_screen_center = { game_screen_x / 2.f, game_screen_y / 2.f };

    Entity player = registry.create();
    registry.add(player, Shield{ shield_max_energy, shield_max_energy, 0.f, BLACK, BLACK, BLACK });
    registry.add(player, PlayerInput{ false, false, false, false });
        registry.add(player, Weapon{
        player_max_ammo,
        0.f,
        weapon_cooldown_period,
        true });
    registry.add(player, PolygonShip{{
        Line{{ -10.f,  +4.f }, {   0.f, -14.f }, RED, 1.5f },
        Line{{   0.f, -14.f }, { +10.f,  +4.f }, BLUE, 1.5f },
        Line{{ +10.f,  +4.f }, {   0.f,   0.f }, GREEN, 1.5f },
        Line{{   0.f,   0.f }, { -10.f,  +4.f }, YELLOW, 1.5f }}, 
        Vector2{ 0.f, -1.f },
        player_max_speed,
        player_acceleration });
    registry.add(player, Transform{
        game_screen_center,
        { 0.f, 0.f },
        player_turn_speed,
        player_drag_coeff });

    std::vector<Entity> asteroids;

    const Vector2 one_third = { game_screen.x / 3.f, game_screen.y / 3.f };
    const Vector2 two_thirds = { 2.f*game_screen.x / 3.f, 2.f*game_screen.y / 3.f };


    for (uint8_t i = 0; i < registry.game_state.starting_asteroid_count; ++i) {
        float a_x = my_rng(0.f, two_thirds.x, Dist::Uniform);
        if (a_x > one_third.x) a_x += one_third.x;

        float a_y = my_rng(0.f, two_thirds.y, Dist::Uniform);
        if (a_y > one_third.y) a_y += one_third.y;

        asteroids.push_back(registry.create());
        float theta = my_rng(0.f, 2.f * PI, Dist::Uniform);
        float dir_x = cos(theta);
        float dir_y = sin(theta);
        registry.add(asteroids.at(i), Size{ asteroid_radius, asteroid_size });
        registry.add(asteroids.at(i), Transform{ { a_x, a_y }, { dir_x * asteroid_init_speed, dir_y * asteroid_init_speed }, 0.f, 1.f });
        registry.add(asteroids.at(i), Asteroid{ generate_asteroid(asteroid_size, asteroid_radius, RED, 1.25f) });
    }

    Entity explosion_sound = registry.create();
    registry.add(explosion_sound, Sound{ });
}

} // end namespace
