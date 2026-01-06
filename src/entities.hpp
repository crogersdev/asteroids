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
        Transform{ initialPosition, 0.f, 0.f },
        Velocity{ Vector2{ 0.f, 0.f }, 0.f },
        PlayerInput{ false, false, false, false };
        Sprite{ LoadTexture("../res/PlayerShip/PlayerShipNormal/Player-Ship3.png"), Rectangle{ 0, 0, 

}

}
