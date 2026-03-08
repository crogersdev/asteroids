#pragma once

#include "components.hpp"

#include <cstdint>
#include <raylib.h>

namespace crogersdev {

using Entity = uint16_t;

static Entity entity_id = 0;

struct Bullet {
    Color color;
    Size size;
    Transform transform;
    Velocity velocity;
};

struct Player {
    PlayerInput input;
    TimesFired timesFired;
    Transform transform;
    Velocity velocity;
    WeaponCooldown cooldown;
};

struct SpritePlayer : Player {
    Sprite shipSprite;
};

struct PolygonPlayer : Player {
    PolygonShip ship;
};

inline SpritePlayer createSpritePlayer(Vector2 initialPosition) {
    return SpritePlayer{
        PlayerInput{ false, false, false, false },
        TimesFired{ 0 },
        Transform{ initialPosition, PI/120.f },
        Velocity{ Vector2{ 0.f, 0.f }, 0.f },
        WeaponCooldown{ 1.5f },
        Sprite{
            LoadTexture("../../res/player.png"),
            Rectangle{ 0, 0, 32, 32 }
        }
    };
};

inline PolygonPlayer createPolygonPlayer(Vector2 initialPosition) {
    float x = initialPosition.x;
    float y = initialPosition.y;
    return PolygonPlayer{
        PlayerInput{ false, false, false, false },
        TimesFired{ 0 },
        Transform{ initialPosition, PI/120.f },
        Velocity{ Vector2{ 0.f, 0.f }, 0.f },
        WeaponCooldown{ 1.5f },
        PolygonShip{{
            Line{{ x-10.f, y+4.f }, { x, y-14.f }, RED },
            Line{{ x, y-14.f }, { x+10.f, y+4.f }, BLUE },
            Line{{ x+10.f, y+4.f }, { x, y }, GREEN },
            Line{{ x, y }, { x-10.f, y+4.f }, YELLOW },
        }}
    };
};

}
