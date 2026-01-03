#include "raylib.h"

#include "entities.hpp"
#include "components.hpp"
#include "systems.hpp"

const bool DEBUG_GAME = false;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

using namespace crogersdev;

void drawDebug() {

}

int main(void) {

    if (DEBUG_GAME) {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids!");
    } else {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DEBUG Asteroids!");
    }

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        BeginDrawing();
            ClearBackground(BLACK);

            if (DEBUG_GAME) {
                drawDebug();
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
