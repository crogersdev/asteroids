#include "systems/systems.hpp"
#include "systems/game-init.hpp"

#include <raylib.h>

using namespace crogersdev;

int main(void) {
    if (DEBUG_GAME) {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DEBUG Asteroids!");
    } else {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids!");
    }
    SetTargetFPS(60);

    assets = std::make_shared<Assets>(); 
    Registry registry = Registry();

    registry.game_state = { 5, 0, 2 };
    game_init(registry);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
            ClearBackground(BLACK);

            if (DEBUG_GAME) { }

            if (registry.game_state.current_state == state_t::MENU) {
                menu_draw_system(registry);
                menu_input_system(registry);
            }
            /*
            if (registry.game_state.current_state == state_t::PAUSED) {
                std::cout << "foo\n";
            }
            if (registry.game_state.current_state == state_t::PLAYING) {

            }
            */
            if (registry.game_state.current_state == state_t::PLAYING) {
                player_input_system(registry);
                bullet_collision_system(registry);
                player_collision_system(registry);
                movement_update_system(registry);
                weapon_system(registry);
                shield_system(registry);
                render_system(registry);
                clear_player_inputs(registry);
            }

            DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
