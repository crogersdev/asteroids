#pragma once

#include "asteroid-generator.hpp"
#include "../components.hpp"
#include "../entities.hpp"

#include <cmath>
#include <random>

#include <raylib.h>

namespace crogersdev {

inline void game_init(Registry& registry) {
    const float x = GetScreenWidth();
    const float y = GetScreenHeight();
    const Vector2 center = { x / 2.f, y / 2.f };

    const float    player_acceleration = 750.f;
    const float    player_invincible_period = 5.f;
    const uint32_t player_max_ammo = 999;
    const float    player_max_energy = 99.f;
    const float    player_max_speed = 425.f;
    const float    player_turn_speed = .08f;
    const float    player_drag_coeff = .995;
    const float    weapon_cooldown_period = 1.5f;

    Entity player = registry.create();
    registry.add(player, Energy{ player_max_energy, player_max_energy });
    registry.add(player, Invincible{ player_invincible_period, 0.f });
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
        { center.x, center.y },
        { 0.f, 0.f },
        player_turn_speed,
        player_drag_coeff });

    std::vector<Entity> asteroids;

    std::random_device rd;
    std::mt19937 gen(rd());
    const Vector2 one_third = { x / 3.f, y / 3.f };
    const Vector2 two_thirds = { 2.f*x / 3.f, 2.f*y / 3.f };
    std::uniform_int_distribution<uint32_t> dist_x(0, two_thirds.x);
    std::uniform_int_distribution<uint32_t> dist_y(0, two_thirds.y);
    std::uniform_real_distribution<float> dist_theta(0, 2*PI);

    const uint32_t asteroid_speed  = 125;
    const uint32_t asteroid_radius = 12;
    const uint32_t asteroid_size   = 4;

    for (uint8_t i = 0; i < registry.game_state.starting_asteroid_count; ++i) {
        float a_x = dist_x(gen);
        if (a_x > one_third.x) a_x += one_third.x;

        float a_y = dist_y(gen);
        if (a_y > one_third.y) a_y += one_third.y;

        asteroids.push_back(registry.create());
        float theta = dist_theta(gen);
        float dir_x = cos(theta);
        float dir_y = sin(theta);
        registry.add(asteroids.at(i), Size{ asteroid_radius, asteroid_size });
        registry.add(asteroids.at(i), Transform{ { a_x, a_y }, { dir_x * asteroid_speed, dir_y * asteroid_speed }, 0.f, 1.f });
        registry.add(asteroids.at(i), Asteroid{ generate_asteroid(asteroid_size, asteroid_radius, RED, 1.25f) });
    }

    Entity explosion_sound = registry.create();
    registry.add(explosion_sound, Sound{ });

}

} // end namespace
