#pragma once

#include "../components.hpp"
#include "../entities.hpp"

#include <cmath>
#include <iostream>

#include <raylib.h>

namespace crogersdev {

inline void clear_player_inputs(Registry& registry) {
    for (Entity& player_id : registry.view<PlayerInput>()) {
        auto& player = registry.get<PlayerInput>(player_id);
        player.thrust = false;
        player.shoot = false;
        player.rotate_left = false;
        player.rotate_right = false;
    }
}

inline void draw_debug_info() {
    DrawCircle(GetScreenWidth() / 2.f, GetScreenHeight() / 2.f, 2.f, WHITE);
}

inline void movement_update_system(Registry& registry) {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();

    for (Entity& e : registry.view<Transform>()) {
        auto& transform = registry.get<Transform>(e);

        Vector2 dt_offset = { transform.velocity.x * GetFrameTime(),
                              transform.velocity.y * GetFrameTime() };

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

    for (Entity& e : registry.view<Transform, PlayerInput, PolygonShip>()) {
        auto& transform = registry.get<Transform>(e);
        auto& player = registry.get<PlayerInput>(e);
        auto& ship = registry.get<PolygonShip>(e);

        if (player.rotate_left || player.rotate_right) {
            Vector2 new_start = {}, new_end = {}, new_orientation = {};
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

            new_orientation.x = ship.orientation.x * cos(t) - ship.orientation.y * sin(t);
            new_orientation.y = ship.orientation.x * sin(t) + ship.orientation.y * cos(t);

            ship.orientation = new_orientation;
        }

        if (player.thrust) {
            transform.velocity.x += ship.orientation.x * ship.acceleration * GetFrameTime();
            transform.velocity.y += ship.orientation.y * ship.acceleration * GetFrameTime();

            auto magnitude = sqrt(pow(transform.velocity.x, 2.f) + pow(transform.velocity.y, 2.f));
            if (magnitude > ship.max_speed) {
                auto theta = atan2(transform.velocity.y, transform.velocity.x);
                transform.velocity.x = cos(theta) * ship.max_speed;
                transform.velocity.y = sin(theta) * ship.max_speed;
            }
        } else {
            // apply drag
            transform.velocity.x *= transform.drag;
            transform.velocity.y *= transform.drag;
        }

        Vector2 dt_offset = { transform.velocity.x * GetFrameTime(),
                              transform.velocity.y * GetFrameTime() };

        transform.position.x += dt_offset.x;
        transform.position.y += dt_offset.y;

        transform.position.x = fmod(fmod(transform.position.x, w) + w, w);
        transform.position.y = fmod(fmod(transform.position.y, h) + h, h);
    }
}

inline void player_input_system(Registry& registry) {
    for (Entity& player_id : registry.view<PlayerInput>()) {
        auto& player = registry.get<PlayerInput>(player_id);

        if (IsKeyDown(KEY_W))     { player.thrust = true; }
        if (IsKeyDown(KEY_A))     { player.rotate_left = true; }
        if (IsKeyDown(KEY_S))     { }
        if (IsKeyDown(KEY_D))     { player.rotate_right = true; }

        if (IsKeyPressed(KEY_SPACE)) { player.shoot = true; }
    }
}

inline void render_system(Registry& registry) {
    // note: raylib's approach is to have global access to raylib managed
    //       resources.  in this case, that's the window to which we render
    //       and draw all our stuff.  that means we can just straight up
    //       call DrawLineEx without any problems or concerns

    for (Entity& e : registry.view<PolygonShip, Transform>()) {
        auto& ship = registry.get<PolygonShip>(e);
        auto& transform = registry.get<Transform>(e);
        Vector2 pos = transform.position;

        Vector2 start = {}, end = {};
        for (auto& ship_edge : ship.lines) {
            start.x = ship_edge.start.x + pos.x;
            start.y = ship_edge.start.y + pos.y;
            end.x = ship_edge.end.x + pos.x;
            end.y = ship_edge.end.y + pos.y;

            DrawLineEx(start, end, ship_edge.thickness, ship_edge.color);
        }
    }

    for (Entity& e : registry.view<Asteroid, Transform>()) {
        auto& asteroid = registry.get<Asteroid>(e);
        auto& transform = registry.get<Transform>(e);
        Vector2 pos = transform.position;

        for (auto& asteroid_edge : asteroid.lines) {
            DrawLineEx({ asteroid_edge.start.x + pos.x, asteroid_edge.start.y + pos.y },
                       { asteroid_edge.end.x   + pos.x, asteroid_edge.end.y   + pos.y },
                       asteroid_edge.thickness,
                       asteroid_edge.color);
        }
    }

    for (Entity& e : registry.view<Bullet, Transform>()) {
        auto& bullet = registry.get<Bullet>(e);
        auto& transform = registry.get<Transform>(e);
        Vector2 pos = transform.position;

        DrawLineEx({ bullet.bullet.start.x + pos.x, bullet.bullet.start.y + pos.y },
                   { bullet.bullet.end.x   + pos.x, bullet.bullet.end.y   + pos.y, },
                   bullet.bullet.thickness,
                   bullet.bullet.color );
    }
}

inline void weapon_system(Registry& registry) {
    for (Entity& e : registry.view<PlayerInput, PolygonShip, TimesFired, Transform, WeaponCooldown>()) {

        auto& player = registry.get<PlayerInput>(e);
        auto& times_fired = registry.get<TimesFired>(e);
        auto& player_transform = registry.get<Transform>(e);

        if (times_fired.count == times_fired.max) {

        }

        if (player.shoot && times_fired.count < times_fired.max) {
            player.shoot = false;
            times_fired.count++;

            auto& ship = registry.get<PolygonShip>(e);
            auto ship_theta = atan2(ship.orientation.y, ship.orientation.x);

            float bullet_speed = 500.f;
            Vector2 bullet_start = Vector2 { cos(ship_theta) * 15.f, sin(ship_theta) * 15.f };
            Vector2 bullet_end = Vector2{ cos(ship_theta) * 20.f, sin(ship_theta) * 20.f };

            Entity bullet = registry.create();
            registry.add(bullet, Bullet{
                Line{ bullet_start, bullet_end, ORANGE, 2.f },
                bullet_speed,
                10.f });
            registry.add(bullet, Transform{
                Vector2{
                    cos(ship_theta) * 15.f + player_transform.position.x,
                    sin(ship_theta) * 15.f + player_transform.position.y },
                Vector2{ cos(ship_theta) * bullet_speed, sin(ship_theta) * bullet_speed },
                0.f,
                1.f });
        }
    }
}

} // end namespace
