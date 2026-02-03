#include "entities.hpp"
#include "components.hpp"
#include "systems.hpp"

#include <raylib.h>

const bool DEBUG_GAME = true;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

using namespace crogersdev;

int main(void) {

    if (DEBUG_GAME) {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DEBUG Asteroids!");
    } else {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids!");
    }
    SetTargetFPS(60);

    PolygonPlayer player = createPolygonPlayer(Vector2{ SCREEN_WIDTH / 2.f, (SCREEN_HEIGHT / 2.f) + 3.f });

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
            ClearBackground(BLACK);

            if (DEBUG_GAME) {
                drawDebugInfo(player);
            }

        playerInputSystem(player);
        playerPositionUpdate(player);
        renderSystem(player);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
