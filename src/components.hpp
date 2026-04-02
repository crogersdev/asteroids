#pragma once

#include <array>
#include <cstdint>
#include <raylib.h>

namespace crogersdev {

struct Line           { Vector2 start, end; Color color; float thickness; };
struct Asteroid       { std::array<Line, 4> lines; };
struct PlayerInput    { bool thrust, shoot, rotate_left, rotate_right; };
struct PolygonShip    { std::array<Line, 4> lines; Vector2 orientation; float max_speed; float acceleration; };
struct Size           { uint32_t size; };
struct TimesFired     { uint32_t value; };
struct Transform      { Vector2 position; Vector2 velocity; float rotation_speed; float drag; };
struct WeaponCooldown { float timer; };

}

