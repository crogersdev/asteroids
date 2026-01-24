#include "entities.hpp"
#include "components.hpp"
#include "systems.hpp"

#include <raylib.h>

const bool DEBUG_GAME = false;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

using namespace crogersdev;
using Entity = std::uint16_t;

void drawDebug() {

}

int main(void) {

    if (DEBUG_GAME) {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DEBUG Asteroids!");
    } else {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids!");
    }
    SetTargetFPS(60);

    Player player = createPlayer(Vector2{ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f });

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
            ClearBackground(BLACK);

            if (DEBUG_GAME) {
                drawDebug();
            }

        renderSystem(player);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
