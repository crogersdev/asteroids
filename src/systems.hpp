#pragma once

#include "entities.hpp"

#include <raylib.h>

namespace crogersdev {

inline void drawDebugInfo(const PolygonPlayer &rPlayer) {
    DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 2.0, WHITE);
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
        auto new_start_x = old_start_x * cos(rotationSpeed) - old_start_y * sin(rotationSpeed);
        auto new_start_y = old_start_x * sin(rotationSpeed) + old_start_y * cos(rotationSpeed);
        auto new_end_x = old_end_x * cos(rotationSpeed) - old_end_y * sin(rotationSpeed);
        auto new_end_y = old_end_x * sin(rotationSpeed) + old_end_y * cos(rotationSpeed);
        new_start_x += center_x;
        new_start_y += center_y;
        old_start_x += center_x;
        old_start_y += center_y;

        shipEdge.start = Vector2{ new_start_x, new_start_y };
        shipEdge.end = Vector2{ new_end_x, new_end_y };
    }
}

inline void playerInputSystem(PolygonPlayer &rPlayer) {
    auto rotationSpeed = rPlayer.transform.rotationSpeed;

    if (IsKeyDown(KEY_W)) {
    }
    if (IsKeyDown(KEY_A)) {
        rPlayer.transform.rotation -= rotationSpeed;
    }
    if (IsKeyDown(KEY_S)) {
    }
    if (IsKeyDown(KEY_D)) {
        rPlayer.transform.rotation += rotationSpeed;
    }

    playerPositionUpdate(rPlayer);
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
