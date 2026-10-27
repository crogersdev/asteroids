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
    Registry r = Registry();
    Entity player_id = r.create();
    GameState gs(player_id);

    r.add(player_id, Shield{ shield_max_energy, shield_max_energy, 0.f, BLACK, BLACK, BLACK });
    r.add(player_id, PlayerInput{ false, false, false, false });
    r.add(player_id, Weapon{
        player_max_ammo,
        0.f,
        weapon_cooldown_period,
        true });
    r.add(player_id, PolygonShip{{
        Line{{ -10.f,  +4.f }, {   0.f, -14.f }, RED, 1.5f },
        Line{{   0.f, -14.f }, { +10.f,  +4.f }, BLUE, 1.5f },
        Line{{ +10.f,  +4.f }, {   0.f,   0.f }, GREEN, 1.5f },
        Line{{   0.f,   0.f }, { -10.f,  +4.f }, YELLOW, 1.5f }}, 
        player_max_speed,
        player_acceleration });
    r.add(player_id, crogersdev::Transform{
        SCREEN_CENTER,
        { 0.f, 0.f },
        atan2(0.f, 1.f),
        player_turn_speed,
        player_drag_coeff });

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
            ClearBackground(BLACK);

            if (DEBUG_GAME) { }

            if (gs.current_state == state_t::MENU) {
                menu_draw(r, gs, assets);
                menu_input(r, gs, assets);
            }
            /*
            if (r.gs.current_state == state_t::PAUSED) {
                std::cout << "foo\n";
            }
            */
            if (gs.current_state == state_t::NEW_GAME) {
                draw_game_start_modal(r, gs, assets);
            }
            if (gs.current_state == state_t::PLAYING) {
                player_input(r, player_id);
                bullet_collision(r, gs);
                player_collision(r, gs, player_id);
                player_scoot_and_rotate(r);
                movement_update(r);
                weapons_fire(r);
                shield_color_update(r);
                player_input(r, player_id, true);
                eraser(r, gs, assets, player_id);
                render(r, gs);
            }
            if (gs.current_state == state_t::DYING) {
                player_dies(r, gs, assets, player_id);
                movement_update(r);
                eraser(r, gs, assets, player_id);
                render(r, gs);
            }
            if (gs.current_state == state_t::LEVEL_START) {
                level_init(r, gs, player_id);
                shield_color_update(r, true);
            }
            if (gs.current_state == state_t::LEVEL_CLEAR) {

            }

            DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
