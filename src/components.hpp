#pragma once

#include "helpers/asteroid-info.hpp"

#include <array>
#include <vector>
#include <raylib.h>

namespace crogersdev {

struct Circle            { Vector2 center; float radius, thickness; Color color; };
struct HUDText           { float font_size; Font font; Color color; Shader shader; };
struct Line              { Vector2 start, end; Color color; float thickness; };
struct Polygon           { std::vector<Line> lines; };

struct Bullet            { Line bullet; float speed, age, lifespan; };
struct DamageMultiplier  { float xdamage; };
struct Dead              { };
struct HUD               { };
struct HUDBar            { float percent_full; Rectangle bar; Color color; };
struct HUDRect           { HUDText hud; Rectangle rec; float padding, corner_radius, thickness; Color color; };
struct Particle          { float age, lifespan, radius; Color color; };
struct PlayerInput       { bool thrust, shoot, rotate_left, rotate_right; };
struct PolygonShip       { std::array<Line, 4> lines; float max_speed, acceleration; };
struct Shield            { float energy_max, energy_remaining, pivot_lerp; Color start, end, pivot_start, pivot_end; };
struct Size              { uint32_t radius; asteroid_size_t size; };
struct Transform         { Vector2 position, velocity; float orientation, rotation_speed, drag, mass; };
struct Weapon            { uint32_t max_ammo; float cooldown_timer, cooldown; bool ready; };

}
