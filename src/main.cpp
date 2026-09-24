#include "helpers/assets-mgr.hpp"
#include "systems/systems.hpp"

#include <raylib.h>

using namespace crogersdev;

int main(void) {
    if (DEBUG_GAME) {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DEBUG Asteroids!");
    } else {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids!");
    }
    SetTargetFPS(60);

    std::shared_ptr<Assets> assets = std::make_shared<Assets>(); 
    Registry registry = Registry();
    Entity player = registry.create();
    GameState game_state(player);

    registry.add(player, Shield{ shield_max_energy, shield_max_energy, 0.f, BLACK, BLACK, BLACK });
    registry.add(player, PlayerInput{ false, false, false, false });
    registry.add(player, Weapon{
        player_max_ammo,
        0.f,
        weapon_cooldown_period,
        true });
    registry.add(player, PolygonShip{{
        Line{{ -10.f,  +4.f }, {   0.f, -14.f }, RED, 1.5f },
        Line{{   0.f, -14.f }, { +10.f,  +4.f }, BLUE, 1.5f },
        Line{{ +10.f,  +4.f }, {   0.f,   0.f }, GREEN, 1.5f },
        Line{{   0.f,   0.f }, { -10.f,  +4.f }, YELLOW, 1.5f }}, 
        Vector2{ 0.f, -1.f },
        player_max_speed,
        player_acceleration });
    registry.add(player, crogersdev::Transform{
        SCREEN_CENTER,
        { 0.f, 0.f },
        player_turn_speed,
        player_drag_coeff });

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
            ClearBackground(BLACK);

            if (DEBUG_GAME) { }

            if (game_state.current_state == state_t::MENU) {
                menu_draw_system(registry, assets, game_state);
                menu_input_system(registry, assets, game_state);
            }
            /*
            if (registry.game_state.current_state == state_t::PAUSED) {
                std::cout << "foo\n";
            }
            if (registry.game_state.current_state == state_t::PLAYING) {

            }
            */
            if (game_state.current_state == state_t::NEW_GAME) {
                draw_game_start_modal(registry, assets, game_state);
            }
            if (game_state.current_state == state_t::PLAYING) {
                player_input_system(registry);
                bullet_collision_system(registry, game_state);
                player_collision_system(registry, game_state);
                movement_update_system(registry);
                weapon_system(registry);
                shield_system(registry);
                render_system(registry);
                clear_player_inputs(registry);
                level_progress_system(registry, game_state);
            }
            if (game_state.current_state == state_t::DYING) {
            
            }
            if (game_state.current_state == state_t::LEVEL_START) {
                level_init_system(registry, game_state);
            }
            if (game_state.current_state == state_t::LEVEL_CLEAR) {

            }

            DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
