#pragma once

#include "constants.hpp"
#include "entities.hpp"

#include <random>

#include <raylib.h>

namespace crogersdev {

inline void drawDebugInfo(const PolygonPlayer &rPlayer) {
    DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 2.0, WHITE);
}

inline void gameInit(Registry& registry, Vector2 screen) {
    float x = screen.x;
    float y = screen.y;
    float c_x = x / 2.f;
    float c_y = y / 2.f;

    Entity player = registry.create();
    registry.add(player, PlayerInput{ false, false, false, false });
    registry.add(player, TimesFired{ 0 });
    registry.add(player, Transform{ Vector2{ c_x, c_y }, PI / 120.f });
    registry.add(player, Velocity{ Vector2{ 0.f, 0.f }, 0.f });
    registry.add(player, WeaponCooldown{ 1.5f });
    registry.add(player, PolygonShip{{
        Line{{ c_x-10.f, c_y+4.f }, { c_x, c_y-14.f }, RED },
        Line{{ c_x, c_y-14.f }, { c_x+10.f, c_y+4.f }, BLUE },
        Line{{ c_x+10.f, c_y+4.f }, { c_x, c_y }, GREEN },
        Line{{ c_x, c_y }, { c_x-10.f, c_y+4.f }, YELLOW },
    }});

    constexpr int initialAsteroids = 5;
    std::array<Entity, initialAsteroids> asteroids;


    std::random_device rd;
    std::mt19937 gen(rd());
    int one_third_x = x / 3;
    int one_third_y = y / 3;
    int two_thirds_x = 2*x / 3;
    int two_thirds_y = 2*y / 3;
    int shift_x = x / 3;
    int shift_y = y / 3;
    std::uniform_int_distribution<int> dist_x(0, two_thirds_x);
    std::uniform_int_distribution<int> dist_y(0, two_thirds_y);

    for (int i = 0; i < initialAsteroids; ++i) {
        float a_x = dist_x(gen);
        if (a_x > one_third_x) a_x += one_third_x;

        float a_y = dist_y(gen);
        if (a_x > one_third_y) a_y += one_third_y;

        asteroids[i] = registry.create();
        registry.add(asteroids[i], Asteroid{{
            Line{{ a_x-.f, a_y+4.f }, { a_x, a_y-14.f }, RED },
            Line{{ a_x-10.f, a_y+4.f }, { a_x, a_y-14.f }, RED },
            Line{{ a_x-10.f, a_y+4.f }, { a_x, a_y-14.f }, RED },
            Line{{ a_x-10.f, a_y+4.f }, { a_x, a_y-14.f }, RED },
            Line{{ a_x-10.f, a_y+4.f }, { a_x, a_y-14.f }, RED },
            Line{{ a_x-10.f, a_y+4.f }, { a_x, a_y-14.f }, RED },
            Line{{ a_x-10.f, a_y+4.f }, { a_x, a_y-14.f }, RED },
            Line{{ a_x-10.f, a_y+4.f }, { a_x, a_y-14.f }, RED },
        }});
    }
}

inline void playerInputSystem(PolygonPlayer &rPlayer) {
    bool input = false;

    if (IsKeyDown(KEY_W)) {
    }
    if (IsKeyDown(KEY_A)) {
        rPlayer.transform.rotationSpeed = -PLAYER_ROTATION_SPEED;
        input = true;
    }
    if (IsKeyDown(KEY_S)) {
    }
    if (IsKeyDown(KEY_D)) {
        rPlayer.transform.rotationSpeed = PLAYER_ROTATION_SPEED;
        input = true;
    }

    if (IsKeyDown(KEY_SPACE)) {
        rPlayer.timesFired.value++;
    }

    if (!input) {
        rPlayer.transform.rotationSpeed = 0.f;
    }
}

inline void playerPositionUpdate(PolygonPlayer &rPlayer) {
    // apply rotation matrix
    auto center_x = rPlayer.transform.position.x;
    auto center_y = rPlayer.transform.position.y;

    for (auto& shipEdge: rPlayer.ship.lines) {
        auto old_start_x = shipEdge.start.x - center_x;
        auto old_start_y = shipEdge.start.y - center_y;
        auto old_end_x = shipEdge.end.x - center_x;
        auto old_end_y = shipEdge.end.y - center_y;

        auto theta = rPlayer.transform.rotationSpeed;

        auto new_start_x = old_start_x * cos(theta) - old_start_y * sin(theta);
        auto new_start_y = old_start_x * sin(theta) + old_start_y * cos(theta);
        auto new_end_x = old_end_x * cos(theta) - old_end_y * sin(theta);
        auto new_end_y = old_end_x * sin(theta) + old_end_y * cos(theta);

        new_start_x += center_x;
        new_start_y += center_y;
        new_end_x += center_x;
        new_end_y += center_y;

        shipEdge.start = Vector2{ new_start_x, new_start_y };
        shipEdge.end = Vector2{ new_end_x, new_end_y };
    }
}

inline void renderSystem(const PolygonPlayer &rPlayer) {
    // note: raylib's approach is to have global access to raylib managed
    //       resources.  in this case, that's the window to which we render
    //       and draw all our stuff.  my initial instinct was to have to pass
    //       in a reference to the window and use that reference
    //       to invoke any DrawFoo type methods but we don't have to do that.

    for (const auto& shipEdge: rPlayer.ship.lines) {
        DrawLineV(shipEdge.start, shipEdge.end, shipEdge.color);
    };
}

}
