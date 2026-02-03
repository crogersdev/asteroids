#pragma once

#include "components.hpp"

#include <cstdint>
#include <raylib.h>

namespace crogersdev {

using Entity = uint16_t;

static Entity entity_id = 0;

struct Player {
    PlayerInput input;
    Transform transform;
    WeaponCooldown cooldown;
    Velocity velocity;
};

struct SpritePlayer : Player {
    Sprite shipSprite;
};

struct PolygonPlayer : Player {
    PolygonShip ship;
};

inline SpritePlayer createSpritePlayer(Vector2 initialPosition) {
    return SpritePlayer{
        PlayerInput{ false, false, false, false },
        Transform{ initialPosition, PI/120.f },
        WeaponCooldown{ 1.5f },
        Velocity{ Vector2{ 0.f, 0.f }, 0.f },
        Sprite{
            LoadTexture("../../res/player.png"),
            Rectangle{ 0, 0, 32, 32 }
        }
    };
};

inline PolygonPlayer createPolygonPlayer(Vector2 initialPosition) {
    float x = initialPosition.x;
    float y = initialPosition.y;
    return PolygonPlayer{
        PlayerInput{ false, false, false, false },
        Transform{ initialPosition, PI/120.f },
        WeaponCooldown{ 1.5f },
        Velocity{ Vector2{ 0.f, 0.f }, 0.f },
        PolygonShip{ {
            Line{{ x-10.f, y+4.f }, { x, y-14.f }, RED },
            Line{{ x, y-14.f }, { x+10.f, y+4.f }, BLUE },
            Line{{ x+10.f, y+4.f }, { x, y }, GREEN },
            Line{{ x, y }, { x-10.f, y+4.f }, YELLOW },
        }
        }
    };
};

}
