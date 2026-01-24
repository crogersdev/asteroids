#pragma once

#include "components.hpp"

#include <cstdint>
#include <raylib.h>

namespace crogersdev {

using Entity = uint16_t;

static Entity entity_id = 0;

struct Player {
    PlayerInput input;
    Transform transform;
    WeaponCooldown cooldown;
    Velocity velocity;
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
        Transform{ initialPosition, 0.f, 0.f },
        WeaponCooldown{ 1.5f },
        Velocity{ Vector2{ 0.f, 0.f }, 0.f },
        Sprite{
            LoadTexture("../../res/player.png"),
            Rectangle{ 0, 0, 32, 32 }
        }
    };

}
