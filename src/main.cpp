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
    Entity player_id = registry.create();
    GameState game_state(player_id);

    registry.add(player_id, Shield{ shield_max_energy, shield_max_energy, 0.f, BLACK, BLACK, BLACK });
    registry.add(player_id, PlayerInput{ false, false, false, false });
    registry.add(player_id, Weapon{
        player_max_ammo,
        0.f,
        weapon_cooldown_period,
        true });
    registry.add(player_id, PolygonShip{{
        Line{{ -10.f,  +4.f }, {   0.f, -14.f }, RED, 1.5f },
        Line{{   0.f, -14.f }, { +10.f,  +4.f }, BLUE, 1.5f },
        Line{{ +10.f,  +4.f }, {   0.f,   0.f }, GREEN, 1.5f },
        Line{{   0.f,   0.f }, { -10.f,  +4.f }, YELLOW, 1.5f }}, 
        Vector2{ 0.f, -1.f },
        player_max_speed,
        player_acceleration });
    registry.add(player_id, crogersdev::Transform{
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
                menu_draw(registry, assets, game_state);
                menu_input(registry, assets, game_state);
            }
            /*
            if (registry.game_state.current_state == state_t::PAUSED) {
                std::cout << "foo\n";
            }
            */
            if (game_state.current_state == state_t::NEW_GAME) {
                draw_game_start_modal(registry, assets, game_state);
            }
            if (game_state.current_state == state_t::PLAYING) {
                player_input(registry, player_id);
                bullet_collision(registry, game_state);
                player_collision(registry, game_state, player_id);
                player_scoot_and_rotate(registry);
                movement_update(registry);
                weapons_fire(registry);
                shield_color_update(registry);
                render(registry);
                level_progress(registry, game_state);
                player_input(registry, player_id, true);
            }
            if (game_state.current_state == state_t::DYING) {
                player_dies(registry, game_state, player_id);
                movement_update(registry);
                render(registry);
            }
            if (game_state.current_state == state_t::LEVEL_START) {
                level_init(registry, game_state);
                shield_color_update(registry, true);
            }
            if (game_state.current_state == state_t::LEVEL_CLEAR) {

            }

            DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
