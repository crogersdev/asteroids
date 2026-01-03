#pragma once

#include <raylib.h>

namespace crogersdev {

struct Transform      { Vector2 position; float rotation; };
struct Velocity       { Vector2 direction; float speed; };
struct PlayerInput    { bool thrust; bool shoot, rotateLeft, rotateRight; };
struct Sprite         { Texture2D texture; Rectangle rectangle; };
struct WeaponCooldown { float timer; };

}

