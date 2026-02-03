#pragma once

#include <array>
#include <cstdint>
#include <raylib.h>

namespace crogersdev {

struct Line           { Vector2 start, end; Color color; };
struct PlayerInput    { bool thrust, shoot, rotateLeft, rotateRight; };
struct PolygonShip    { std::array<Line, 4> lines; };
struct Size           { uint32_t size; };
struct Sprite         { Texture2D texture; Rectangle rectangle; };
struct Transform      { Vector2 position; float rotationSpeed; };
struct Velocity       { Vector2 direction; float speed; };
struct WeaponCooldown { float timer; };

}

