#pragma once

#include "constants.hpp"
#include "entities.hpp"

#include <random>

#include <raymath.h>
#include <raylib.h>

namespace crogersdev {

inline void drawDebugInfo() {
    DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 2.0, WHITE);
}

inline void gameInit(Registry& registry) {
    float x = GetScreenWidth();
    float y = GetScreenHeight();
    float c_x = x / 2.f;
    float c_y = y / 2.f;

    Entity player = registry.create();
    registry.add(player, PlayerInput{ false, false, false, false });
    registry.add(player, TimesFired{ 0 });
    registry.add(player, Transform{ Vector2{ c_x, c_y }, PI / 120.f });
    registry.add(player, Velocity{ Vector2{ 0.f, 0.f }, 0.f });
    registry.add(player, WeaponCooldown{ 1.5f });
    registry.add(player, PolygonShip{{
        Line{{ c_x-10.f, c_y+4.f },  { c_x,      c_y-14.f }, RED },
        Line{{ c_x,      c_y-14.f }, { c_x+10.f, c_y+4.f },  BLUE },
        Line{{ c_x+10.f, c_y+4.f },  { c_x,      c_y },      GREEN },
        Line{{ c_x,      c_y },      { c_x-10.f, c_y+4.f },  YELLOW },
    }});

    constexpr int initialAsteroids = 5;
    std::array<Entity, initialAsteroids> asteroids;

    std::random_device rd;
    std::mt19937 gen(rd());
    int one_third_x = x / 3;
    int one_third_y = y / 3;
    int two_thirds_x = 2*x / 3;
    int two_thirds_y = 2*y / 3;
    std::uniform_int_distribution<int> dist_x(0, two_thirds_x);
    std::uniform_int_distribution<int> dist_y(0, two_thirds_y);
    std::uniform_real_distribution<float> dist_theta(0, 2*PI);

    for (int i = 0; i < initialAsteroids; ++i) {
        float a_x = dist_x(gen);
        if (a_x > one_third_x) a_x += one_third_x;

        float a_y = dist_y(gen);
        if (a_y > one_third_y) a_y += one_third_y;

        asteroids[i] = registry.create();
        registry.add(asteroids[i], Asteroid{{
            Line{{ a_x-10.f, a_y+10.f }, { a_x-10.f, a_y-10.f }, RED },
            Line{{ a_x-10.f, a_y-10.f }, { a_x+10.f, a_y-10.f }, RED },
            Line{{ a_x+10.f, a_y-10.f }, { a_x+10.f, a_y+10.f }, RED },
            Line{{ a_x+10.f, a_y+10.f }, { a_x-10.f, a_y+10.f }, RED },
        }});

        registry.add(asteroids[i], Transform{ Vector2{ a_x, a_y }, 0.0 });

        float theta = dist_theta(gen);
        float dir_x = cos(theta);
        float dir_y = sin(theta);
        registry.add(asteroids[i], Velocity{ Vector2{ dir_x, dir_y }, 100.f });
    }
}

inline void playerInputSystem(Registry& registry) {
    for (Entity& player_id : registry.view<PlayerInput>()) {
        auto& player = registry.get<PlayerInput>(player_id);

        if (IsKeyDown(KEY_W)) {
            player.thrust = true;
        }
        if (IsKeyDown(KEY_A)) {
            player.rotateLeft = true;
        }
        if (IsKeyDown(KEY_S)) {
        }
        if (IsKeyDown(KEY_D)) {
            player.rotateRight = true;
        }
        if (IsKeyDown(KEY_SPACE)) {
            player.shoot = true;
        }
    }
}

inline void clearPlayerInputs(Registry& registry) {
    for (Entity& player_id : registry.view<PlayerInput>()) {
        auto& player = registry.get<PlayerInput>(player_id);
        player.thrust = false;
        player.shoot = false;
        player.rotateLeft = false;
        player.rotateRight = false;
    }
}

inline void fireFrequencySystem(Registry& registry) {
    for (Entity& shooter : registry.view<TimesFired>()) {
        // todo: you're going to have to add a clock system
        //       and component and when that's done you can
        //       like, idk, allow fire frequency to reset.
    }
}

inline void movementUpdateSystem(Registry& registry) {
    for (Entity& e : registry.view<Transform, Velocity>()) {
        auto& t = registry.get<Transform>(e);
        auto& v = registry.get<Velocity>(e);

        t.position.x += v.direction.x * v.speed * GetFrameTime();
        t.position.y += v.direction.y * v.speed * GetFrameTime();
    }

    for (Entity& e : registry.view<Transform, Velocity, PlayerInput, PolygonShip>()) {
        auto& t = registry.get<Transform>(e);
        auto& v = registry.get<Velocity>(e);
        auto& p = registry.get<PlayerInput>(e);
        auto& player = registry.get<PolygonShip>(e);

        auto center = t.position;

        for(auto& shipEdge : player.lines) {
            auto old_start = Vector2{
                shipEdge.start.x - center.x,
                shipEdge.start.y - center.y };

            auto old_end = Vector2{
                shipEdge.end.x - center.x,
                shipEdge.end.y - center.y };

            Vector2 newStart = Vector2{};
            Vector2 oldStart = Vector2{};

            if (p.rotateLeft || p.rotateRight) {
                auto theta = t.rotationSpeed;
                if (p.rotateRight) { theta *= -1.f; }
                auto new_start = Vector2{
                    old_start.x * cos(theta) - old_start.y * sin(theta),
                    old_start.x * sin(theta) + old_start.y * cos(theta) };

                auto new_end = Vector2{
                    old_end.x * cos(theta) - old_end.y * sin(theta),
                    old_end.x * sin(theta) + old_end.y * cos(theta) };

                new_start.x += center.x;
                new_start.y += center.y;
                new_end.x += center.x;
                new_end.y += center.y;

                shipEdge.start = new_start;
                shipEdge.end = new_end;
            }

            if (p.thrust) {

            }

            if (p.shoot) {
            }
        }
    }
}

inline void renderSystem(Registry& registry) {
    // note: raylib's approach is to have global access to raylib managed
    //       resources.  in this case, that's the window to which we render
    //       and draw all our stuff.  that means we can just straight up
    //       call DrawLineV without any problems or concerns.

    for (Entity& e : registry.view<PolygonShip>()) {
        auto& ship = registry.get<PolygonShip>(e);
        for (auto& shipEdge : ship.lines) {
            DrawLineV(shipEdge.start, shipEdge.end, shipEdge.color);
        }
    }
    for (Entity& e : registry.view<Asteroid>()) {
        auto& asteroid = registry.get<Asteroid>(e);
        for (auto& asteroidEdge : asteroid.lines) {
            DrawLineV(asteroidEdge.start, asteroidEdge.end, asteroidEdge.color);
        }
    }
}

