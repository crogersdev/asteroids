#pragma once

#include <array>
#include <cstdint>
#include <raylib.h>

namespace crogersdev {

struct Line           { Vector2 start, end; Color color; float thickness; };
struct Asteroid       { std::array<Line, 4> lines; };
struct Bullet         { Line bullet; float speed; float lifetime; float max_lifetime; };
struct PlayerInput    { bool thrust, shoot, rotate_left, rotate_right; };
struct PolygonShip    { std::array<Line, 4> lines; Vector2 orientation; float max_speed, acceleration; };
struct Size           { uint32_t size; };
struct Transform      { Vector2 position; Vector2 velocity; float rotation_speed; float drag; };
struct Weapon         { uint32_t max_ammo; float freq_timer, freq, cooldown_timer, cooldown; };

}

