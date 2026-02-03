#pragma once

#include "entities.hpp"

#include <raylib.h>

namespace crogersdev {

inline void drawDebugInfo(const PolygonPlayer &rPlayer) {
    DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 2.0, WHITE);
}

inline void playerInputSystem(PolygonPlayer &rPlayer) {
    bool input = false;

    if (IsKeyDown(KEY_W)) {
    }
    if (IsKeyDown(KEY_A)) {
        rPlayer.transform.rotationSpeed = -PI/120.f;
        input = true;
    }
    if (IsKeyDown(KEY_S)) {
    }
    if (IsKeyDown(KEY_D)) {
        rPlayer.transform.rotationSpeed = PI/120.f;
        input = true;
    }

    if (!input) {
        rPlayer.transform.rotationSpeed = 0.f;
    }
}

inline void playerPositionUpdate(PolygonPlayer &rPlayer) {
    // apply rotation matrix
    auto center_x = rPlayer.transform.position.x;
    auto center_y = rPlayer.transform.position.y;
    auto rotationSpeed = rPlayer.transform.rotationSpeed;

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
