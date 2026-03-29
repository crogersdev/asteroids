#pragma once

#include "entities.hpp"

#include <iostream>
#include <random>

#include <raylib.h>

namespace crogersdev {

inline void draw_debug_info() {
    DrawCircle(GetScreenWidth() / 2.f, GetScreenHeight() / 2.f, 2.f, WHITE);
}

inline void game_init(Registry& registry) {
    float x = GetScreenWidth();
    float y = GetScreenHeight();
    Vector2 center = { x / 2.f, y / 2.f };

    float player_thrust_speed = 125.f;
    float player_turn_speed = .05f;
    float player_shoot_rate = 1.5f;

    Entity player = registry.create();
    registry.add(player, PlayerInput{ false, false, false, false });
    registry.add(player, TimesFired{ 0 });
    registry.add(player, Transform{ { center.x, center.y }, player_turn_speed });
    registry.add(player, Velocity{ { 0.f, -1.f }, 125.f });
    registry.add(player, WeaponCooldown{ player_shoot_rate });
    registry.add(player, PolygonShip{{
        Line{{ -10.f,  +4.f }, {   0.f, -14.f }, RED },
        Line{{   0.f, -14.f }, { +10.f,  +4.f }, BLUE },
        Line{{ +10.f,  +4.f }, {   0.f,   0.f }, GREEN },
        Line{{   0.f,   0.f }, { -10.f,  +4.f }, YELLOW },
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
        registry.add(asteroids[i], Transform{ { a_x, a_y }, 0.f });
        registry.add(asteroids[i], Size{ 4 });
        registry.add(asteroids[i], Asteroid{{
            Line{{ -40.f, +40.f }, { -40.f, -40.f }, RED },
            Line{{ -40.f, -40.f }, { +40.f, -40.f }, RED },
            Line{{ +40.f, -40.f }, { +40.f, +40.f }, RED },
            Line{{ +40.f, +40.f }, { -40.f, +40.f }, RED },
        }});

        float theta = dist_theta(gen);
        float dir_x = cos(theta);
        float dir_y = sin(theta);
        registry.add(asteroids[i], Velocity{ { dir_x, dir_y }, 100.f });
    }
}

inline void player_input_system(Registry& registry) {
    for (Entity& player_id : registry.view<PlayerInput>()) {
        auto& player = registry.get<PlayerInput>(player_id);

        if (IsKeyDown(KEY_W))     { player.thrust = true; }
        if (IsKeyDown(KEY_A))     { player.rotate_left = true; }
        if (IsKeyDown(KEY_S))     { }
        if (IsKeyDown(KEY_D))     { player.rotate_right = true; }
        if (IsKeyDown(KEY_SPACE)) { player.shoot = true; }
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
        //       like, idk, allow fire frequency to reset.uo9 ikn,m 
    }
}

inline void movement_update_system(Registry& registry) {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();

    for (Entity& e : registry.view<Asteroid, Transform, Velocity>()) {
        auto& transform = registry.get<Transform>(e);
        auto& velocity = registry.get<Velocity>(e);

        Vector2 dt_offset = { velocity.direction.x * velocity.speed * GetFrameTime(),
                              velocity.direction.y * velocity.speed * GetFrameTime() };

        transform.position.x += dt_offset.x;
        transform.position.y += dt_offset.y;

        // explain: if this is confusing, it's because we're using fmod to 
        //          screen wrap but in a way that respects negative numbers
        //          e.g.
        //          x = -10, screen_width = 800
        //          fmod(-10, 800) = -10
        //          -10 + 800 = 790
        //          fmod(790, 800) = 790
        //          ^^^ if asteroid goes off left edge by 10.  rework with
        //          positive 10 and you'll see you correctly get 10 with the same
        //          code
        transform.position.x = fmod(fmod(transform.position.x, w) + w, w);
        transform.position.y = fmod(fmod(transform.position.y, h) + h, h);
    }

    for (Entity& e : registry.view<Transform, Velocity, PlayerInput, PolygonShip>()) {
        auto& transform = registry.get<Transform>(e);
        auto& velocity = registry.get<Velocity>(e);
        auto& player = registry.get<PlayerInput>(e);
        auto& ship = registry.get<PolygonShip>(e);

        if (player.rotate_left || player.rotate_right) {
            Vector2 new_start = {}, new_end = {};
            float t = transform.rotation_speed;
            if (player.rotate_left) { t *= -1.f; }

            for (auto& ship_edge : ship.lines) {
                new_start.x = ship_edge.start.x * cos(t) - ship_edge.start.y * sin(t);
                new_start.y = ship_edge.start.x * sin(t) + ship_edge.start.y * cos(t);

                new_end.x = ship_edge.end.x * cos(t) - ship_edge.end.y * sin(t);
                new_end.y = ship_edge.end.x * sin(t) + ship_edge.end.y * cos(t);

                ship_edge.start = new_start;
                ship_edge.end   = new_end;
            }

            Vector2 new_direction = {};
            new_direction.x = velocity.direction.x * cos(t) - velocity.direction.y * sin(t);
            new_direction.y = velocity.direction.x * sin(t) + velocity.direction.y * cos(t);

            velocity.direction = new_direction;
        }

        if (player.thrust) {
            Vector2 dt_offset = { velocity.direction.x * velocity.speed * GetFrameTime(),
                                  velocity.direction.y * velocity.speed * GetFrameTime() };

            transform.position.x += dt_offset.x;
            transform.position.y += dt_offset.y;

            transform.position.x = fmod(fmod(transform.position.x, w) + w, w);
            transform.position.y = fmod(fmod(transform.position.y, h) + h, h);
        }

        if (player.shoot) {
        }
    }
}

inline void render_system(Registry& registry) {
    // note: raylib's approach is to have global access to raylib managed
    //       resources.  in this case, that's the window to which we render
    //       and draw all our stuff.  that means we can just straight up
    //       call DrawLineEx without any problems or concerns
    float line_thickness = 1.5f;

    for (Entity& e : registry.view<PolygonShip, Transform>()) {
        auto& ship = registry.get<PolygonShip>(e);
        auto& transform = registry.get<Transform>(e);

        Vector2 start = {}, end = {};
        for (auto& ship_edge : ship.lines) {
            start.x = ship_edge.start.x + transform.position.x;
            start.y = ship_edge.start.y + transform.position.y;
            end.x = ship_edge.end.x + transform.position.x;
            end.y = ship_edge.end.y + transform.position.y;

            DrawLineEx(start, end, line_thickness, ship_edge.color);
        }
    }

    for (Entity& e : registry.view<Asteroid, Transform>()) {
        auto& transform = registry.get<Transform>(e);
        auto& asteroid = registry.get<Asteroid>(e);
        for (auto& asteroidEdge : asteroid.lines) {
            DrawLineEx({ asteroidEdge.start.x + transform.position.x, asteroidEdge.start.y + transform.position.y },
                       { asteroidEdge.end.x + transform.position.x, asteroidEdge.end.y + transform.position.y },
                       line_thickness,
                       asteroidEdge.color);
        }
    }
}

} // end namespace
