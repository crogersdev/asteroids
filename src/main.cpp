#include "systems/systems.hpp"
#include "systems/game-init.hpp"

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

    Registry registry = Registry();

    game_init(registry);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
            ClearBackground(BLACK);

            if (DEBUG_GAME) {
            }

            player_input_system(registry);
            collision_system(registry);
            movement_update_system(registry);
            weapon_system(registry);
            render_system(registry);
            clear_player_inputs(registry);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
