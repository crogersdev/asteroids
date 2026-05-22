#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <raylib.h>

namespace crogersdev {

struct Line              { Vector2 start, end; Color color; float thickness; };

struct Asteroid          { std::vector<Line> lines; };
struct Bullet            { Line bullet; float speed, age, lifespan; };
struct Energy            { float max, remaining; };
struct Invincible        { float max, time_remaining; };
struct Particle          { float age, lifespan, radius; Color color; };
struct PlayerInput       { bool thrust, shoot, rotate_left, rotate_right; };
struct PolygonShip       { std::array<Line, 4> lines; Vector2 orientation; float max_speed, acceleration; };
struct Size              { uint32_t radius, size; };
struct Sound             { bool playing; std::string path; };
struct Transform         { Vector2 position, velocity; float rotation_speed, drag; };
struct Weapon            { uint32_t max_ammo; float cooldown_timer, cooldown; bool ready; };

}
