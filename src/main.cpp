#include "raylib.h"

#include <vector>
#include <algorithm>   // std::clamp

const bool DEBUG_GAME = false;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

void drawDebug() {
    
}

int main(void) {

    if (DEBUG_GAME) {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Falling Tunnel");
    } else {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DEBUG Falling Tunnel");
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
