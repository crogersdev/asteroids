#pragma once

#include "entities.hpp"

#include <random>

#include <raylib.h>

namespace crogersdev {

inline void draw_debug_info() {
    DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 2.0, WHITE);
}

inline void game_init(Registry& registry) {
    float x = GetScreenWidth();
    float y = GetScreenHeight();
    Vector2 center = { x / 2.f, y / 2.f };

    Entity player = registry.create();
    registry.add(player, PlayerInput{ false, false, false, false });
    registry.add(player, TimesFired{ 0 });
    registry.add(player, Transform{ { center.x, center.y }, PI / 120.f });
    registry.add(player, Velocity{ { 0.f, 0.f }, 0.f });
    registry.add(player, WeaponCooldown{ 1.5f });
    registry.add(player, PolygonShip{{
        Line{{ center.x-10.f, center.y+4.f },  { center.x,      center.y-14.f }, RED },
        Line{{ center.x,      center.y-14.f }, { center.x+10.f, center.y+4.f },  BLUE },
        Line{{ center.x+10.f, center.y+4.f },  { center.x,      center.y },      GREEN },
        Line{{ center.x,      center.y },      { center.x-10.f, center.y+4.f },  YELLOW },
    }});

    constexpr int initialAsteroids = 5;
    std::array<Entity, initialAsteroids> asteroids;

    std::random_device rd;
    std::mt19937 gen(rd());
    Vector2 one_third = { x / 3.f, y / 3.f };
    Vector2 two_thirds = { 2.f*x / 3.f, 2.f*y / 3.f };
    std::uniform_int_distribution<int> dist_x(0, two_thirds.x);
    std::uniform_int_distribution<int> dist_y(0, two_thirds.y);
    std::uniform_real_distribution<float> dist_theta(0, 2*PI);

    for (int i = 0; i < initialAsteroids; ++i) {
        float a_x = dist_x(gen);
        if (a_x > one_third.x) a_x += one_third.x;

        float a_y = dist_y(gen);
        if (a_y > one_third.y) a_y += one_third.y;

        asteroids[i] = registry.create();
        registry.add(asteroids[i], Asteroid{{
            Line{{ a_x-10.f, a_y+10.f }, { a_x-10.f, a_y-10.f }, RED },
            Line{{ a_x-10.f, a_y-10.f }, { a_x+10.f, a_y-10.f }, RED },
            Line{{ a_x+10.f, a_y-10.f }, { a_x+10.f, a_y+10.f }, RED },
            Line{{ a_x+10.f, a_y+10.f }, { a_x-10.f, a_y+10.f }, RED },
        }});

        registry.add(asteroids[i], Transform{ { a_x, a_y }, 0.0 });

        float theta = dist_theta(gen);
        float dir_x = cos(theta);
        float dir_y = sin(theta);
        registry.add(asteroids[i], Velocity{ { dir_x, dir_y }, 100.f });
    }
}

inline void player_input_system(Registry& registry) {
    for (Entity& player_id : registry.view<PlayerInput>()) {
        auto& player = registry.get<PlayerInput>(player_id);

        if (IsKeyDown(KEY_W)) {
            player.thrust = true;
        }
        if (IsKeyDown(KEY_A)) {
            player.rotate_left = true;
        }
        if (IsKeyDown(KEY_S)) {
        }
        if (IsKeyDown(KEY_D)) {
            player.rotate_right = true;
        }
        if (IsKeyDown(KEY_SPACE)) {
            player.shoot = true;
        }
    }
}

inline void clear_player_inputs(Registry& registry) {
    for (Entity& player_id : registry.view<PlayerInput>()) {
        auto& player = registry.get<PlayerInput>(player_id);
        player.thrust = false;
        player.shoot = false;
        player.rotate_left = false;
        player.rotate_right = false;
    }
}

inline void fire_frequency_system(Registry& registry) {
    for (Entity& shooter : registry.view<TimesFired>()) {
        // todo: you're going to have to add a clock system
        //       and component and when that's done you can
        //       like, idk, allow fire frequency to reset.
    }
}

inline void movement_update_system(Registry& registry) {
    for (Entity& e : registry.view<Asteroid, Transform, Velocity>()) {
        auto& t = registry.get<Transform>(e);
        auto& v = registry.get<Velocity>(e);

        t.position.x += v.direction.x * v.speed * GetFrameTime();
        t.position.y += v.direction.y * v.speed * GetFrameTime();

        auto& asteroid = registry.get<Asteroid>(e);
        for (auto& edge : asteroid.lines) {
            Vector2 start = { edge.start.x + t.position.x, edge.start.y + t.position.y };
            Vector2 end   = { edge.end.x   + t.position.x, edge.end.y   + t.position.y };
        }
    }

    for (Entity& e : registry.view<Transform, Velocity, PlayerInput, PolygonShip>()) {
        auto& t = registry.get<Transform>(e);
        auto& v = registry.get<Velocity>(e);
        auto& p = registry.get<PlayerInput>(e);
        auto& player = registry.get<PolygonShip>(e);

        auto center = t.position;

        for(auto& ship_edge : player.lines) {
            Vector2 old_start = {
                ship_edge.start.x - center.x,
                ship_edge.start.y - center.y };

            Vector2 old_end = {
                ship_edge.end.x - center.x,
                ship_edge.end.y - center.y };

            if (p.rotate_left || p.rotate_right) {
                auto theta = t.rotation_speed;
                if (p.rotate_left) { theta *= -1.f; }
                Vector2 new_start = {
                    old_start.x * cos(theta) - old_start.y * sin(theta),
                    old_start.x * sin(theta) + old_start.y * cos(theta) };

                Vector2 new_end = {
                    old_end.x * cos(theta) - old_end.y * sin(theta),
                    old_end.x * sin(theta) + old_end.y * cos(theta) };

                new_start.x += center.x;
                new_start.y += center.y;
                new_end.x += center.x;
                new_end.y += center.y;

                ship_edge.start = new_start;
                ship_edge.end = new_end;
            }

            if (p.thrust) {

            }

            if (p.shoot) {
            }
        }
    }
}

inline void render_system(Registry& registry) {
    // note: raylib's approach is to have global access to raylib managed
    //       resources.  in this case, that's the window to which we render
    //       and draw all our stuff.  that means we can just straight up
    //       call DrawLineV without any problems or concerns.

    for (Entity& e : registry.view<PolygonShip>()) {
        auto& ship = registry.get<PolygonShip>(e);
        for (auto& ship_edge : ship.lines) {
            DrawLineV(ship_edge.start, ship_edge.end, ship_edge.color);
        }
    }
    for (Entity& e : registry.view<Asteroid>()) {
        auto& asteroid = registry.get<Asteroid>(e);
        for (auto& asteroidEdge : asteroid.lines) {
            DrawLineV(asteroidEdge.start, asteroidEdge.end, asteroidEdge.color);
        }
    }
}

} // end namespace
