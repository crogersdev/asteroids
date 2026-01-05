#pragma once

#include "components.hpp"

#include <cstdint>
#include <raylib.h>

namespace crogersdev {

using Entity = uint16_t;

static Entity entity_id = 0;

struct Player {
    Transform transform;
    Velocity velocity;
    PlayerInput input;
    Sprite sprite;
    WeaponCooldown cooldown;
};

inline Player createPlayer(Vector2 initialPosition, Vector2) {
    return Player{
        Transform{ Vector2{ SCREEN_WIDTH / 2.f, SCREEN_HEIGHTjkkk
    };
}

}
