#pragma once

#include <raylib.h>

struct Transform   { Vector2 position; float rotation; };
struct Velocity    { Vector2 direction; float speed; };
struct PlayerInput { bool thrust; bool shoot, rotateLeft, rotateRight; };

struct Sprite { Texture2D }

struct Player { Transform transform; Velocity velocity; int lives; float shootCooldown; };
struct Asteroid { Transform transform; Velocity velocity; float size;

