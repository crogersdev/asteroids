#pragma once

#include "asteroid-generator.hpp"
#include "../constants.hpp"
#include "../components.hpp"
#include "../constants.hpp"
#include "../entities.hpp"
#include "../helpers/assets-mgr.hpp"
#include "../helpers/game-state.hpp"
#include "../helpers/math-stuff.hpp"
#include "../helpers/physics.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <tuple>
#include <raylib.h>
#include <raymath.h>

namespace crogersdev {

inline void bullet_collision_system(Registry& registry, GameState& game_state) {
    std::vector<Entity> dead_asteroids;
    std::vector<Entity> dead_bullets;
    std::vector<Entity> dead_particles;

    // TODO: If you're going to make bullets long lines, you'll need
    // to track collision through leading edge or possibly all points along the line
    // in order to preserve things like an asteroid crossing that line
    for (Entity bullet_id : registry.view<Bullet, Transform>()) {
        const auto& bullet_transform = registry.get<Transform>(bullet_id);

        for (Entity asteroid_id : registry.view<AsteroidShape, Size, Transform>()) {
            const auto& asteroid_transform = registry.get<Transform>(asteroid_id);
            const auto& asteroid_size = registry.get<Size>(asteroid_id);
            const uint32_t sz = static_cast<uint32_t>(asteroid_size.size);

            auto asteroid_collision_radius = asteroid_size.radius * sz;
            auto bullet_distance_to_asteroid =
                pow(bullet_transform.position.x - asteroid_transform.position.x, 2) +
                pow(bullet_transform.position.y - asteroid_transform.position.y, 2);

            if (bullet_distance_to_asteroid <= pow(asteroid_collision_radius, 2)) {
                for (int p = 0; p < particle_max; p++) {
                    auto particle_theta = my_rng(0.f, 2.f * PI, Dist::Uniform);
                    auto particle_speed = my_rng(250.f, 50.f, Dist::Normal);
                    auto particle_radius = my_rng(3.5f, .5f, Dist::Normal);
                    auto particle_lifespan = my_rng(.1f, .75f, Dist::Normal);

                    Entity particle = registry.create();
                    registry.add(particle, Particle{
                        particle_age,
                        particle_lifespan,
                        particle_radius,
                        WHITE });
                    registry.add(particle, Transform{
                        asteroid_transform.position,
                        { cos(particle_theta) * particle_speed, sin(particle_theta) * particle_speed },
                        0.f,
                        particle_drag,
                        0.f });
                }

                if (asteroid_size.size > asteroid_size_t::TINY) {
                    auto parent_speed = sqrt( pow(asteroid_transform.velocity.x, 2.f) + pow(asteroid_transform.velocity.y, 2.f));
                    int child_asteroids = 2;
                    for (int i = 0; i < child_asteroids; i++) {
                        Entity new_asteroid = registry.create();
                        float new_theta = my_rng(0.f, 2.f * PI, Dist::Uniform); // angle(gen);
                        float new_speed = my_rng(1.f, 1.f, Dist::Normal) * parent_speed;
                        new_speed = std::clamp(new_speed, asteroid_init_speed * .5f, asteroid_init_speed * 3.f);

                        registry.add(new_asteroid, Size{ asteroid_size.radius, asteroid_size.size-1 });
                        registry.add(new_asteroid, Transform{
                            asteroid_transform.position,
                            { cos(new_theta) * new_speed, sin(new_theta) * new_speed },
                            0.f,
                            1.f,
                            0.f });
                        registry.add(
                            new_asteroid,
                            AsteroidShape{ generate_asteroid_shape(asteroid_size.size-1, asteroid_size.radius, RED, 1.25f) });
                        game_state.remaining_asteroids++;
                    }
                }

                dead_asteroids.push_back(asteroid_id);
                dead_bullets.push_back(bullet_id);
            }
        }
    }

    for (Entity particle_id : registry.view<Particle>()) {
        auto& particle_info = registry.get<Particle>(particle_id);

        particle_info.age += GetFrameTime();

        if (particle_info.age >= particle_info.lifespan) {
            dead_particles.push_back(particle_id);
        }
    }

    for (auto b : dead_bullets) {
        registry.destroy(b);
    }
    for (auto a : dead_asteroids) {
        game_state.remaining_asteroids--;
        registry.destroy(a);
    }
    for (auto p : dead_particles) {
        registry.destroy(p);
    }
}

inline void clear_player_inputs(Registry& registry) {
    for (Entity player_id : registry.view<PlayerInput>()) {
        auto& player = registry.get<PlayerInput>(player_id);
        player.thrust = false;
        player.shoot = false;
        player.rotate_left = false;
        player.rotate_right = false;
    }
}

inline void draw_debug_info() {
    DrawCircle(GetScreenWidth() / 2.f, GetScreenHeight() / 2.f, 2.f, WHITE);
}

inline void draw_game_start_modal(Registry& registry, std::shared_ptr<Assets> assets, GameState& game_state) {
    assets->game_start_animation_timer.current_time += GetFrameTime();

    if (assets->game_start_animation_timer.current_time >= 4.0f) {
        game_state.current_state = state_t::LEVEL_START;
        assets->game_start_animation_timer.current_time = 0.f;
        return;
    }

    BeginBlendMode(BLEND_ALPHA);
    BeginShaderMode(assets->title_font_shader);    
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 200 });

    float local_timer = fmod(assets->game_start_animation_timer.current_time, 1.0f);
    float scale;

    if (local_timer < 0.8f) {
        float animationProgress = local_timer / .8f;
        scale = animationProgress * animationProgress;
    } else {
        scale = 1.f;
    }

    Color c = INDIGO;

    std::string countdown;
    if (assets->game_start_animation_timer.current_time < 1.0f) countdown = "3";
    else if (assets->game_start_animation_timer.current_time < 2.0f) countdown = "2";
    else if (assets->game_start_animation_timer.current_time < 3.0f) { countdown = "1"; c = HOT_PINK; }
    else countdown = "GO!";

    float font_size = assets->menu_title_font.baseSize * scale;
    Vector2 textSize = MeasureTextEx(assets->menu_title_font, countdown.c_str(), font_size, 2);
    Vector2 center = Vector2{ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f };
    Vector2 pos = Vector2{ center.x - textSize.x / 2.f, center.y - textSize.y / 2.f };

    DrawTextEx(assets->menu_title_font, countdown.c_str(), pos, assets->menu_title_font.baseSize * scale, 2.0f, c);
    EndShaderMode();
    EndBlendMode();
}

inline void level_clear_system(Registry& registry, GameState& game_state) {

}

inline void level_init_system(Registry& registry, GameState& game_state) {
    const Vector2 one_third          = { SCREEN_WIDTH / 3.f, SCREEN_HEIGHT / 3.f };
    const Vector2 two_thirds         = { 2.f*SCREEN_WIDTH / 3.f, 2.f*SCREEN_HEIGHT / 3.f };

    std::vector<Entity> asteroids;
    for (uint8_t i = 0; i < game_state.remaining_asteroids; ++i) {
        float a_x = my_rng(0.f, two_thirds.x, Dist::Uniform);
        if (a_x > one_third.x) a_x += one_third.x;

        float a_y = my_rng(0.f, two_thirds.y, Dist::Uniform);
        if (a_y > one_third.y) a_y += one_third.y;

        asteroids.push_back(registry.create());
        float theta = my_rng(0.f, 2.f * PI, Dist::Uniform);
        float dir_x = cos(theta);
        float dir_y = sin(theta);
        registry.add(asteroids.at(i), Size{ asteroid_radius, asteroid_size_t::LARGE });
        registry.add(asteroids.at(i), Transform{ { a_x, a_y }, { dir_x * asteroid_init_speed, dir_y * asteroid_init_speed }, 0.f, 1.f, 0.f });
        registry.add(asteroids.at(i), AsteroidShape{ generate_asteroid_shape(asteroid_size_t::LARGE, asteroid_radius, RED, 1.25f) });
    }

    game_state.current_state = state_t::PLAYING;
}

inline void level_progress_system(Registry& registry, GameState& game_state) {
    float corner = .1f;
    Vector2 asteroids_remaining_hud = { SCREEN_WIDTH - SCREEN_WIDTH * corner, SCREEN_HEIGHT - SCREEN_HEIGHT * corner };
    std::string remaining = std::to_string(game_state.remaining_asteroids);
    DrawTextEx(GetFontDefault(), remaining.c_str(), asteroids_remaining_hud, 32.f, 1.f, MY_GOLD);
}

inline void menu_draw_system(Registry& registry, std::shared_ptr<Assets> assets, GameState& game_state) {
    float menu_title_font_size = 150.f;
    float menu_option_font_size = 48.f;

    Vector2 title_bounding_box = MeasureTextEx(assets->menu_title_font, std::string(menu_title).c_str(), menu_title_font_size, 2.f); 
    float horizontal_margin = (SCREEN_WIDTH - title_bounding_box.x) / 2.f;
    DrawTextEx(assets->menu_title_font, std::string(menu_title).c_str(), { horizontal_margin, 40.f }, menu_title_font_size, 2.f, CYAN);

    auto current_selection = game_state.menu_selected_option;
    Color menu_option_color;

    current_selection == menu_options_t::START ? [&](){ menu_option_color = HOT_PINK; }() : [&](){ menu_option_color = CYAN; }();
    Vector2 menu_options_box = MeasureTextEx(assets->menu_option_font, "start", menu_option_font_size, 2.f);
    float vertical_margin = SCREEN_HEIGHT / 2.f;
    horizontal_margin = (SCREEN_WIDTH - menu_options_box.x) / 2.f;
    DrawTextEx(assets->menu_option_font, "start", { horizontal_margin, vertical_margin }, menu_option_font_size, 2.f, menu_option_color);

    current_selection == menu_options_t::SETTINGS ? [&](){ menu_option_color = HOT_PINK; }() : [&](){ menu_option_color = CYAN; }();
    menu_options_box = MeasureTextEx(assets->menu_option_font, "settings", menu_option_font_size, 2.f);
    vertical_margin += menu_option_font_size + menu_option_font_size * .5f;
    horizontal_margin = (SCREEN_WIDTH - menu_options_box.x) / 2.f;
    DrawTextEx(assets->menu_option_font, "settings", { horizontal_margin, vertical_margin }, menu_option_font_size, 2.f, menu_option_color);

    current_selection == menu_options_t::QUIT ? [&](){ menu_option_color = HOT_PINK; }() : [&](){ menu_option_color = CYAN; }();
    menu_options_box = MeasureTextEx(assets->menu_option_font, "quit", menu_option_font_size, 2.f);
    vertical_margin += menu_option_font_size + menu_option_font_size * .5f;
    horizontal_margin = (SCREEN_WIDTH - menu_options_box.x) / 2.f;
    DrawTextEx(assets->menu_option_font, "quit", { horizontal_margin, vertical_margin }, menu_option_font_size, 2.f, menu_option_color);
}

inline void menu_input_system(Registry& registry, std::shared_ptr<Assets> assets, GameState& game_state) {
    if (IsKeyPressed(KEY_DOWN)) { game_state.nextMenuOption(game_state.menu_selected_option); }
    if (IsKeyPressed(KEY_UP))   { game_state.prevMenuOption(game_state.menu_selected_option); }

    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        switch (game_state.menu_selected_option) {
        case menu_options_t::START:
            game_state.current_state = state_t::NEW_GAME;
            break;
        case menu_options_t::SETTINGS:
            game_state.current_state = state_t::UPDATE_SETTINGS;
            break;
        case menu_options_t::QUIT:
            game_state.current_state = state_t::QUITTING;
            break;
        }
    }
}

inline void movement_update_system(Registry& registry) {
    auto w = SCREEN_WIDTH;
    auto h = SCREEN_HEIGHT;

    Entity e = registry.view<Transform, PlayerInput, PolygonShip>().front();
    auto& input = registry.get<PlayerInput>(e);
    auto& ship = registry.get<PolygonShip>(e);
    auto& player_transform = registry.get<Transform>(e);

    if (input.rotate_left || input.rotate_right) {
        Vector2 new_start = {}, new_end = {}, new_orientation = {};
        float t = player_transform.rotation_speed;
        if (input.rotate_left) { t *= -1.f; }

        for (auto& ship_edge : ship.lines) {
            new_start.x = ship_edge.start.x * cos(t) - ship_edge.start.y * sin(t);
            new_start.y = ship_edge.start.x * sin(t) + ship_edge.start.y * cos(t);

            new_end.x = ship_edge.end.x * cos(t) - ship_edge.end.y * sin(t);
            new_end.y = ship_edge.end.x * sin(t) + ship_edge.end.y * cos(t);

            ship_edge.start = new_start;
            ship_edge.end   = new_end;
        }

        new_orientation.x = ship.orientation.x * cos(t) - ship.orientation.y * sin(t);
        new_orientation.y = ship.orientation.x * sin(t) + ship.orientation.y * cos(t);

        ship.orientation = new_orientation;
    }

    if (input.thrust) {
        player_transform.velocity.x += ship.orientation.x * ship.acceleration * GetFrameTime();
        player_transform.velocity.y += ship.orientation.y * ship.acceleration * GetFrameTime();

        auto magnitude = sqrt(pow(player_transform.velocity.x, 2.f) + pow(player_transform.velocity.y, 2.f));
        if (magnitude > ship.max_speed) {
            auto theta = atan2(player_transform.velocity.y, player_transform.velocity.x);
            player_transform.velocity.x = cos(theta) * ship.max_speed;
            player_transform.velocity.y = sin(theta) * ship.max_speed;
        }
    }

    for (Entity e : registry.view<Transform>()) {
        auto& transform = registry.get<Transform>(e);

        const Vector2 dt_offset = { transform.velocity.x * transform.drag * GetFrameTime(),
                                    transform.velocity.y * transform.drag * GetFrameTime() };

        transform.position.x += dt_offset.x;
        transform.position.y += dt_offset.y;

        // explain: if this is confusing, it's because we're using fmod to 
        //          screen wrap but in a way that respects negative numbers
        //          e.g.
        //          x = -10, screen_width = 800
        //          fmod(-10, 800) = -10
        //          -10 + 800 = 790
        //          fmod(790, 800) = 790
        //          ^^^ if asteroid goes off left edge by 10.  rework with
        //          positive 10 and you'll see you correctly get 10 with the same
        //          code
        transform.position.x = fmod(fmod(transform.position.x, w) + w, w);
        transform.position.y = fmod(fmod(transform.position.y, h) + h, h);
    }
}
inline void player_collision_system(Registry& registry, GameState& game_state) {
    Entity ship_id = registry.view<PolygonShip, Shield, Transform>().front();
    const auto& ship = registry.get<PolygonShip>(ship_id);
    auto& ship_transform = registry.get<Transform>(ship_id);

    auto& ship_shield = registry.get<Shield>(ship_id);

    for (Entity asteroid_id : registry.view<AsteroidShape, Size, Transform>()) {
        const auto& asteroid_size = registry.get<Size>(asteroid_id);
        const uint32_t sz = static_cast<uint32_t>(asteroid_size.size);

        auto& asteroid_transform = registry.get<Transform>(asteroid_id);

        float asteroid_ship_distance_val = Vector2DistanceSqr(ship_transform.position, asteroid_transform.position);
        auto asteroid_collision_radius = asteroid_size.radius * sz;
        auto ship_shield_collision_radius = shield_radius + shield_thickness;

        if (asteroid_ship_distance_val > pow(ship_shield_collision_radius + asteroid_collision_radius, 2)) {
            colliding_objects.erase(std::make_pair(ship_id, asteroid_id));
            continue;
        }

        auto p = std::make_pair(ship_id, asteroid_id);

        if (colliding_objects.count(p)) { continue; }  // if we're already colliding during this frame, skip.

        colliding_objects.insert(p);

        if (ship_shield.energy_remaining <= sz * asteroid_damage) {
            game_state.lives--;
            if (game_state.lives > 0) {
                game_state.current_state = state_t::DYING;
            } else {
                game_state.current_state = state_t::GAME_OVER;
            }
        }

        if (asteroid_size.size == asteroid_size_t::LARGE) {
            std::tie(ship_transform.velocity, asteroid_transform.velocity) = compute_collision_velocities(ship_transform.velocity, asteroid_transform.velocity, ship_transform.position, asteroid_transform.position, 10.f, 20.f);
            ship_shield.energy_remaining -= sz * asteroid_damage;
        } else if (asteroid_size.size == asteroid_size_t::MEDIUM) {
            std::tie(ship_transform.velocity, asteroid_transform.velocity) = compute_collision_velocities(ship_transform.velocity, asteroid_transform.velocity, ship_transform.position, asteroid_transform.position, 10.f, 20.f);
            ship_shield.energy_remaining -= sz * asteroid_damage / 2.f;
        } else if (asteroid_size.size == asteroid_size_t::SMALL) {
            std::tie(ship_transform.velocity, asteroid_transform.velocity) = compute_collision_velocities(ship_transform.velocity, asteroid_transform.velocity, ship_transform.position, asteroid_transform.position, 10.f, 8.f);
            ship_shield.energy_remaining -= sz * asteroid_damage / 4.f;
        } else if (asteroid_size.size == asteroid_size_t::TINY) {
            std::tie(ship_transform.velocity, asteroid_transform.velocity) = compute_collision_velocities(ship_transform.velocity, asteroid_transform.velocity, ship_transform.position, asteroid_transform.position, 10.f, 1.f);
            ship_shield.energy_remaining -= sz * asteroid_damage / 10.f;
        }
    } // end for each asteroid
}

inline void player_input_system(Registry& registry) {
    Entity player_id = registry.view<PlayerInput>().front();
    auto& player = registry.get<PlayerInput>(player_id);

    if (IsKeyDown(KEY_W))        { player.thrust = true; }
    if (IsKeyDown(KEY_A))        { player.rotate_left = true; }
    if (IsKeyDown(KEY_S))        { }
    if (IsKeyDown(KEY_D))        { player.rotate_right = true; }
    if (IsKeyPressed(KEY_SPACE)) { player.shoot = true; }
}

inline void sound_system(Registry& registry) {
}

inline void shield_system(Registry& registry) {
    for (Entity shield_id : registry.view<Shield>()) {
        auto& shield = registry.get<Shield>(shield_id);
        if (shield.energy_remaining > 0.f) {
            auto palette = neon_synth_palette;
            if (shield.energy_remaining <= shield.energy_max && shield.energy_remaining >= shield.energy_max * .666f) {
                shield.pivot_start = palette.full.start;
                shield.pivot_end   = palette.full.end,
                shield.pivot_lerp  = 1.f - normalize(shield.energy_remaining, shield.energy_max * .666f, shield.energy_max);
            } else if (shield.energy_remaining < shield.energy_max * .666f && shield.energy_remaining >= shield.energy_max * .333f) {
                shield.pivot_start = palette.half.start;
                shield.pivot_end   = palette.half.end;
                shield.pivot_lerp  = 1.f - normalize(shield.energy_remaining, shield.energy_max * .333f, shield.energy_max * .666f);
            } else {
                shield.pivot_start = palette.dead.start;
                shield.pivot_end   = palette.dead.end;
                shield.pivot_lerp  = 1.f - normalize(shield.energy_remaining, 0.f, shield.energy_max * .333f);
            }
        }
    }
}

inline void render_system(Registry& registry) {
    // note: raylib's approach is to have global access to raylib managed
    //       resources.  in this case, that's the window to which we render
    //       and draw all our stuff.  that means we can just straight up
    //       call DrawLineEx without any problems or concerns

    Entity ship_id = registry.view<PolygonShip, Shield, Transform>().front();
    const auto& ship = registry.get<PolygonShip>(ship_id);
    const auto& transform = registry.get<Transform>(ship_id);

    Vector2 pos = transform.position;
    auto& shield = registry.get<Shield>(ship_id);
    if (shield.energy_remaining > 0) {
        DrawRing(pos, shield_radius, shield_radius + shield_thickness, 0, 360, 36, ColorLerp(shield.pivot_start, shield.pivot_end, shield.pivot_lerp));
    }

    Vector2 start = {}, end = {};
    for (const auto& ship_edge : ship.lines) {
        start.x = ship_edge.start.x + pos.x;
        start.y = ship_edge.start.y + pos.y;
        end.x = ship_edge.end.x + pos.x;
        end.y = ship_edge.end.y + pos.y;

        DrawLineEx(start, end, ship_edge.thickness, ship_edge.color);
    }

    Vector2 arrow = Vector2Scale(Vector2Normalize(transform.velocity), 40.f);
    // DrawLineEx(pos, Vector2Add(pos, arrow), 2.f, HOT_PINK);

    for (Entity asteroid_id : registry.view<AsteroidShape, Transform>()) {
        const auto& asteroid_shape = registry.get<AsteroidShape>(asteroid_id);
        const auto& transform = registry.get<Transform>(asteroid_id);
        Vector2 pos = transform.position;

        for (const auto& edge : asteroid_shape.lines) {
            DrawLineEx(
                { edge.start.x + pos.x, edge.start.y + pos.y },
                { edge.end.x   + pos.x, edge.end.y   + pos.y },
                edge.thickness,
                edge.color);
        }
        Vector2 arrow = Vector2Scale(Vector2Normalize(transform.velocity), 40.f);
        // DrawLineEx(pos, Vector2Add(pos, arrow), 2.f, HOT_PINK);
    }

    for (Entity bullet_id : registry.view<Bullet, Transform>()) {
        const auto& bullet = registry.get<Bullet>(bullet_id);
        const auto& transform = registry.get<Transform>(bullet_id);
        Vector2 pos = transform.position;

        DrawLineEx({ bullet.bullet.start.x + pos.x, bullet.bullet.start.y + pos.y },
                   { bullet.bullet.end.x   + pos.x, bullet.bullet.end.y   + pos.y, },
                   bullet.bullet.thickness,
                   bullet.bullet.color );
    }

    for (Entity particle_id : registry.view<Particle, Transform>()) {
        const auto& particle = registry.get<Particle>(particle_id);
        const auto& particle_transform = registry.get<Transform>(particle_id);

        DrawCircleGradient(particle_transform.position, particle.radius, particle.color, BLACK);
    }
}

inline void weapon_system(Registry& registry) {
    float bullet_speed = 500.f;
    float bullet_offset_from_ship = 15.f;
    float bullet_length = 11.f;
    float bullet_age = 0.f;
    float bullet_lifespan = .75f;
    Vector2 bullet_start = Vector2{ 0.f, 0.f };

    for (Entity e : registry.view<PlayerInput, PolygonShip, Transform, Weapon>()) {
        const auto& ship = registry.get<PolygonShip>(e);
        const auto& player_transform = registry.get<Transform>(e);
        auto& input = registry.get<PlayerInput>(e);
        auto& weapon = registry.get<Weapon>(e);

        if (input.shoot && weapon.ready) {

            weapon.cooldown_timer += (1.f / weapon.max_ammo) * weapon.cooldown;
            if (weapon.cooldown_timer >= weapon.cooldown) { weapon.ready = false; }

            input.shoot = false;

            Vector2 bullet_end = Vector2{
                ship.orientation.x * bullet_length,
                ship.orientation.y * bullet_length };

            Entity bullet = registry.create();
            registry.add(bullet, Bullet{
                Line{ bullet_start, bullet_end, ORANGE, 2.5f },
                bullet_speed,
                bullet_age,
                bullet_lifespan });
            registry.add(bullet, Transform{
                Vector2{
                    ship.orientation.x * (bullet_offset_from_ship + (bullet_length / 2.f)) + player_transform.position.x,
                    ship.orientation.y * (bullet_offset_from_ship + (bullet_length / 2.f)) + player_transform.position.y },
                Vector2{
                    ship.orientation.x * bullet_speed,
                    ship.orientation.y * bullet_speed },
                0.f,
                1.f,
                0.f });
        }

        if (weapon.cooldown_timer >  0.f) { weapon.cooldown_timer -= GetFrameTime(); }
        if (weapon.cooldown_timer <= 0.f) { weapon.ready = true; }
    }

    std::vector<Entity> dead_bullets;
    for (Entity bullet_id : registry.view<Bullet>()) {
        auto& bullet = registry.get<Bullet>(bullet_id);

        bullet.age += GetFrameTime();

        if (bullet.age >= bullet.lifespan) {
            dead_bullets.push_back(bullet_id);
        }
    }

    for (auto b : dead_bullets) {
        registry.destroy(b);
    }
}

} // end namespace
