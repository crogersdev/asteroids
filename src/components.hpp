#pragma once

#include <cstdint>
#include <raylib.h>

namespace crogersdev {

struct Transform      { Vector2 position; float rotation, rotationSpeed; };
struct Velocity       { Vector2 direction; float speed; };
struct PlayerInput    { bool thrust, shoot, rotateLeft, rotateRight; };
struct Size           { uint32_t size; };
struct Sprite         { Texture2D texture; Rectangle rectangle; };
struct WeaponCooldown { float timer; };

}

