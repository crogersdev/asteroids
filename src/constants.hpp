#pragma once

#include <cstdint>

#include <raylib.h>

namespace crogersdev {

// Game Values

inline constexpr uint32_t asteroid_init_speed = 125;
inline constexpr uint32_t asteroid_radius     = 12;
inline constexpr uint32_t asteroid_size       = 4;

inline constexpr float    particle_age  = 0.f;
inline constexpr uint32_t particle_max  = 50;
inline constexpr float    particle_drag = 0.999f;

inline constexpr float    player_acceleration = 750.f;
inline constexpr float    player_invincible_period = 5.f;
inline constexpr uint32_t player_max_ammo = 999;
inline constexpr float    player_max_energy = 99.f;
inline constexpr float    player_max_speed = 425.f;
inline constexpr float    player_turn_speed = .08f;
inline constexpr float    player_drag_coeff = .995;

inline constexpr float    weapon_cooldown_period = 1.5f;

// Reds
const Color CRIMSON        = Color{ 220,  20,  60, 255 };
const Color FIRE_BRICK     = Color{ 178,  34,  34, 255 };
const Color DARK_RED       = Color{ 139,   0,   0, 255 };
const Color SALMON         = Color{ 250, 128, 114, 255 };
const Color CORAL          = Color{ 255, 127,  80, 255 };
const Color TOMATO         = Color{ 255,  99,  71, 255 };

// Oranges / Yellows
const Color DARK_ORANGE    = Color{ 255, 140,   0, 255 };
const Color MY_GOLD        = Color{ 255, 215,   0, 255 };
const Color AMBER          = Color{ 255, 191,   0, 255 };

// Greens
const Color LIME_GREEN     = Color{  50, 205,  50, 255 };
const Color FOREST_GREEN   = Color{  34, 139,  34, 255 };
const Color EMERALD        = Color{  80, 200, 120, 255 };
const Color MINT           = Color{  62, 180, 137, 255 };
const Color CHARTREUSE     = Color{ 127, 255,   0, 255 };

// Blues
const Color SKY_BLUE       = Color{ 135, 206, 235, 255 };
const Color STEEL_BLUE     = Color{  70, 130, 180, 255 };
const Color ROYAL_BLUE     = Color{  65, 105, 225, 255 };
const Color NAVY           = Color{   0,   0, 128, 255 };
const Color CYAN           = Color{   0, 255, 255, 255 };
const Color TEAL           = Color{   0, 128, 128, 255 };

// Purples / Pinks
const Color HOT_PINK       = Color{ 255, 105, 180, 255 };
const Color ORCHID         = Color{ 218, 112, 214, 255 };
const Color INDIGO         = Color{  75,   0, 130, 255 };

// Neutrals
const Color SLATE_GRAY     = Color{ 112, 128, 144, 255 };
const Color DIM_GRAY       = Color{ 105, 105, 105, 255 };
const Color SILVER         = Color{ 192, 192, 192, 255 };
const Color IVORY          = Color{ 255, 255, 240, 255 };
const Color SMOKE          = Color{ 245, 245, 245, 255 };

}
