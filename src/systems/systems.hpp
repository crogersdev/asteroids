#pragma once

#include "asteroid-generator.hpp"
#include "particle-generator.hpp"
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

inline void bullet_collision(Registry& r, GameState& gs) {
    // TODO: If you're going to make bullets long lines, you'll need
    // to track collision through leading edge or possibly all points along the line
    // in order to preserve things like an asteroid crossing that line
    for (Entity bullet_id : r.view<Bullet, Transform, Registry::Exclude<Dead>>()) {
        const auto& bullet_transform = r.get<Transform>(bullet_id);

        for (Entity asteroid_id : r.view<AsteroidShape, Size, Transform, Registry::Exclude<Dead>>()) {
            const auto& asteroid_transform = r.get<Transform>(asteroid_id);
            const auto& asteroid_size = r.get<Size>(asteroid_id);
            const uint32_t sz = static_cast<uint32_t>(asteroid_size.size);

            auto asteroid_collision_radius = asteroid_size.radius * sz;
            auto bullet_distance_to_asteroid =
                pow(bullet_transform.position.x - asteroid_transform.position.x, 2) +
                pow(bullet_transform.position.y - asteroid_transform.position.y, 2);

            if (bullet_distance_to_asteroid <= pow(asteroid_collision_radius, 2)) {
                generate_particles(r, asteroid_transform.position);

                if (asteroid_size.size > asteroid_size_t::TINY) {
                    auto parent_speed = sqrt( pow(asteroid_transform.velocity.x, 2.f) + pow(asteroid_transform.velocity.y, 2.f));
                    int child_asteroids = 2;
                    for (int i = 0; i < child_asteroids; i++) {
                        float new_speed = my_rng(1.f, 1.f, Dist::Normal) * parent_speed;
                        new_speed = std::clamp(new_speed, asteroid_init_speed * .5f, asteroid_init_speed * 3.f);
                        float new_theta = my_rng(0.f, 2.f * PI, Dist::Uniform);

                        spawn_asteroid(
                            r,
                            r.create(),
                            asteroid_size.radius,
                            asteroid_size.size-1,
                            asteroid_transform.position,
                            { cosf(new_theta) * new_speed, sinf(new_theta) * new_speed },
                            new_theta);

                        gs.remaining_asteroids++;
                    }
                }

                r.add(asteroid_id, Dead{});
                r.add(bullet_id, Dead{});
            }
        }
    }
}

inline void clear_level(Registry& r, GameState& gs) {

}

inline void draw_debug_info() {
    DrawCircle(GetScreenWidth() / 2.f, GetScreenHeight() / 2.f, 2.f, WHITE);
}

inline void draw_game_start_modal(Registry& r, GameState& gs, std::shared_ptr<Assets> assets) {
    assets->game_start_animation_timer.current_time += GetFrameTime();

    if (assets->game_start_animation_timer.current_time >= 4.0f) {
        gs.current_state = state_t::LEVEL_START;
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
    const Vector2 textSize = MeasureTextEx(assets->menu_title_font, countdown.c_str(), font_size, 2);
    const Vector2 center = Vector2{ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f };
    const Vector2 pos = Vector2{ center.x - textSize.x / 2.f, center.y - textSize.y / 2.f };

    DrawTextEx(assets->menu_title_font, countdown.c_str(), pos, assets->menu_title_font.baseSize * scale, 2.0f, c);
    EndShaderMode();
    EndBlendMode();
}

inline void level_init(Registry& r, GameState& gs, Entity player_id) {
    auto& pt = r.get<Transform>(player_id);
    pt.position = { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f };
    pt.velocity = { 0.f, 0.f };
    pt.orientation = atan2(0.f, 1.f);

    spawn_asteroids(r, gs, asteroids_per_level[gs.level]);
    spawn_asteroids_for_level(r, gs, );
    
    r.remove<Dead>(player_id);
    gs.current_state = state_t::PLAYING;
}

inline void eraser(Registry& r, GameState& gs, std::shared_ptr<Assets> assets, Entity player_id) {
    std::vector<Entity> dead_particles;

    for (Entity particle_id : r.view<Particle>()) {
        // NOTE: I should refactor this to the asteroid blows up logic and then attach the dead tag
        //       but ... eh.
        auto& particle_info = r.get<Particle>(particle_id);
        particle_info.age += GetFrameTime();
        if (particle_info.age >= particle_info.lifespan) { dead_particles.push_back(particle_id); }
    }

    if (assets->player_resurrection_timer.current_time >= assets->player_resurrection_timer.max_time) {
        gs.current_state = state_t::LEVEL_START;
        assets->player_resurrection_timer.current_time = 0.f;
        for (Entity dse : r.view<Line, Transform, Dead>()) { r.destroy(dse); }
    }

    for (Entity a : r.view<AsteroidShape, Size, Transform, Dead>()) {
        gs.remaining_asteroids--;
        r.destroy(a);
    }

    for (Entity b : r.view<Bullet, Dead>()) { r.destroy(b); }
    for (Entity p : dead_particles) { r.destroy(p); }
}

inline void menu_draw(Registry& r, GameState& gs, std::shared_ptr<Assets> assets) {
    float menu_title_font_size = 150.f;
    float menu_option_font_size = 48.f;

    Vector2 title_bounding_box = MeasureTextEx(assets->menu_title_font, std::string(menu_title).c_str(), menu_title_font_size, 2.f); 
    float horizontal_margin = (SCREEN_WIDTH - title_bounding_box.x) / 2.f;
    DrawTextEx(assets->menu_title_font, std::string(menu_title).c_str(), { horizontal_margin, 40.f }, menu_title_font_size, 2.f, CYAN);

    auto current_selection = gs.menu_selected_option;
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

inline void menu_input(Registry& r, GameState& gs, std::shared_ptr<Assets> assets) {
    if (IsKeyPressed(KEY_DOWN)) { gs.nextMenuOption(gs.menu_selected_option); }
    if (IsKeyPressed(KEY_UP))   { gs.prevMenuOption(gs.menu_selected_option); }

    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        switch (gs.menu_selected_option) {
        case menu_options_t::START:
            gs.current_state = state_t::NEW_GAME;
            break;
        case menu_options_t::SETTINGS:
            gs.current_state = state_t::UPDATE_SETTINGS;
            break;
        case menu_options_t::QUIT:
            gs.current_state = state_t::QUITTING;
            break;
        }
    }
}

inline void movement_update(Registry& r) {
    auto w = SCREEN_WIDTH;
    auto h = SCREEN_HEIGHT;

    for (Entity e : r.view<Transform>()) {
        auto& transform = r.get<Transform>(e);
        const Vector2 dt_offset = Vector2{
            transform.velocity.x * transform.drag * GetFrameTime(),
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

inline void player_collision(Registry& r, GameState& gs, Entity player_id) {
    const auto& ship = r.get<PolygonShip>(player_id);
    auto& ship_transform = r.get<Transform>(player_id);
    auto& ship_shield = r.get<Shield>(player_id);

    for (Entity asteroid_id : r.view<AsteroidShape, Size, Transform, Registry::Exclude<Dead>>()) {
        const auto& asteroid_size = r.get<Size>(asteroid_id);
        const uint32_t sz = static_cast<uint32_t>(asteroid_size.size);

        auto& asteroid_transform = r.get<Transform>(asteroid_id);

        float asteroid_ship_distance_val = Vector2DistanceSqr(ship_transform.position, asteroid_transform.position);
        auto asteroid_collision_radius = asteroid_size.radius * sz;
        auto ship_shield_collision_radius = shield_radius + shield_thickness;

        if (asteroid_ship_distance_val > pow(ship_shield_collision_radius + asteroid_collision_radius, 2)) {
            colliding_objects.erase(std::make_pair(player_id, asteroid_id));
            continue;
        }

        auto p = std::make_pair(player_id, asteroid_id);

        if (colliding_objects.count(p)) { continue; }  // if we're already colliding during this frame, skip.

        colliding_objects.insert(p);

        if (ship_shield.energy_remaining <= sz * asteroid_damage) {
            gs.lives--;
            if (gs.lives > 0) {
                gs.current_state = state_t::DYING;
                return;
            } else {
                gs.current_state = state_t::GAME_OVER;
                return;
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

inline void player_dies(Registry& r, GameState& gs, std::shared_ptr<Assets> assets, Entity player_id) {
    auto dead_ship_edges = r.view<Line, Transform>();
    if (!dead_ship_edges.empty()) { 
        assets->player_resurrection_timer.current_time += GetFrameTime();

        for (Entity dead_ship_id : dead_ship_edges) {
            auto& dead_ship_edge_transform = r.get<Transform>(dead_ship_id);
            float t = dead_ship_edge_transform.rotation_speed * GetFrameTime();
            dead_ship_edge_transform.orientation += t;
        }
    } else {
        r.add(player_id, Dead{ });
        auto& ship_transform = r.get<Transform>(player_id);
        auto& ship = r.get<PolygonShip>(player_id);

        generate_particles(r, ship_transform.position, atan2(ship_transform.velocity.y, ship_transform.velocity.x), PI/8.f, 200);
        for (int i = 0; i < 4; i++) {
            Entity dead_ship_edge = r.create();
            r.add(dead_ship_edge, Dead{});

            auto world_position_start = Vector2Add(ship.lines[i].start, ship_transform.position);
            auto world_position_end   = Vector2Add(ship.lines[i].end,   ship_transform.position);

            Vector2 new_position = Vector2Lerp(
                Vector2Add(ship.lines[i].start, ship_transform.position),
                Vector2Add(ship.lines[i].end,   ship_transform.position),
                .5f);

            auto local_position_start = Vector2Subtract(world_position_start, new_position);
            auto local_position_end   = Vector2Subtract(world_position_end,   new_position);

            r.add(dead_ship_edge, Line{
               local_position_start,
               local_position_end,
               ship.lines[i].color,
               ship.lines[i].thickness
            });

            r.add(dead_ship_edge, Transform{
                new_position,
                { ship_transform.velocity.x * my_rng(-PI/6.f, PI/6.f, Dist::Uniform), ship_transform.velocity.y * my_rng(-PI/6.f, PI/6.f, Dist::Uniform) }, 
                ship_transform.orientation * my_rng(.75f, 1.25f, Dist::Uniform),
                my_rng(1.f, 3.f, Dist::Uniform) * (my_rng(0, 1, Dist::Uniform) == 0 ? -1.f : 1.f),
                .25f,  // drag of 1.f means nothing held back on velocity computation
                0.f
            });
        }
    }
}

inline void player_input(Registry& r, Entity player_id, bool clear = false) {
    auto& player = r.get<PlayerInput>(player_id);

    if (IsKeyDown(KEY_W))        { player.thrust = true; }
    if (IsKeyDown(KEY_A))        { player.rotate_left = true; }
    if (IsKeyDown(KEY_S))        { }
    if (IsKeyDown(KEY_D))        { player.rotate_right = true; }
    if (IsKeyPressed(KEY_SPACE)) { player.shoot = true; }

    if (clear) {
        player.thrust = false;
        player.shoot = false;
        player.rotate_left = false;
        player.rotate_right = false;
    }
}

inline void player_scoot_and_rotate(Registry& r) {
    auto w = SCREEN_WIDTH;
    auto h = SCREEN_HEIGHT;

    auto player_vec = r.view<Transform, PlayerInput, PolygonShip, Registry::Exclude<Dead>>();
    if (player_vec.empty()) { return; }
    Entity player_id = player_vec.front();

    auto& input = r.get<PlayerInput>(player_id);
    auto& ship = r.get<PolygonShip>(player_id);
    auto& pt = r.get<Transform>(player_id);

    if (input.thrust) {
        pt.velocity += Vector2{
            cosf(pt.orientation - PI/2.f) * ship.acceleration * GetFrameTime(),
            sinf(pt.orientation - PI/2.f) * ship.acceleration * GetFrameTime()
        };

        auto magnitude = sqrt(pow(pt.velocity.x, 2.f) + pow(pt.velocity.y, 2.f));
        if (magnitude > ship.max_speed) {
            auto theta = atan2(pt.velocity.y, pt.velocity.x);
            pt.velocity.x = cosf(theta) * ship.max_speed;
            pt.velocity.y = sinf(theta) * ship.max_speed;
        }
    }

    if (input.rotate_left || input.rotate_right) {
        float t = pt.rotation_speed * GetFrameTime();
        if (input.rotate_left) { t *= -1.f; }
        pt.orientation += t;
    }
}

inline void sound_player(Registry& r) {

}

inline void shield_color_update(Registry& r, bool reset = false) {
    Entity shield_id = r.view<Shield>().front();
    auto& shield = r.get<Shield>(shield_id);

    if (reset) { shield.energy_remaining = shield_max_energy; }

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

inline void render(Registry& r, GameState gs) {
    // note: raylib's approach is to have global access to raylib managed
    //       resources.  in this case, that's the window to which we render
    //       and draw all our stuff.  that means we can just straight up
    //       call DrawLineEx without any problems or concerns

    for (Entity ship_id : r.view<PolygonShip, Shield, Transform, Registry::Exclude<Dead>>()) {
        const auto& ship = r.get<PolygonShip>(ship_id);
        const auto& transform = r.get<Transform>(ship_id);

        Vector2 pos = transform.position;
        auto& shield = r.get<Shield>(ship_id);
        if (shield.energy_remaining > 0) {
            DrawRing(pos, shield_radius, shield_radius + shield_thickness, 0, 360, 36, ColorLerp(shield.pivot_start, shield.pivot_end, shield.pivot_lerp));
        }

        Vector2 start = {}, end = {};
        for (const auto& ship_edge : ship.lines) {
            start = Vector2Rotate(ship_edge.start, transform.orientation);
            end   = Vector2Rotate(ship_edge.end,   transform.orientation);

            start = Vector2Add(start, pos);
            end   = Vector2Add(end, pos);

            DrawLineEx(start, end, ship_edge.thickness, ship_edge.color);
        }
    }

    for (Entity dead_ship_edge_id : r.view<Line, Transform>()) {
        const auto& dead_ship_edge = r.get<Line>(dead_ship_edge_id);
        const auto& dead_ship_edge_transform = r.get<Transform>(dead_ship_edge_id);

        Vector2 start = {}, end = {};
        auto pos = dead_ship_edge_transform.position;

        start = Vector2Rotate(dead_ship_edge.start, dead_ship_edge_transform.orientation);
        end   = Vector2Rotate(dead_ship_edge.end,   dead_ship_edge_transform.orientation);

        start = Vector2Add(start, pos);
        end   = Vector2Add(end,   pos);

        DrawLineEx(start, end, dead_ship_edge.thickness, dead_ship_edge.color);
    }

    for (Entity asteroid_id : r.view<AsteroidShape, Transform>()) {
        const auto& asteroid_shape = r.get<AsteroidShape>(asteroid_id);
        const auto& transform = r.get<Transform>(asteroid_id);
        Vector2 pos = transform.position;

        for (const auto& edge : asteroid_shape.lines) {
            DrawLineEx(
                { edge.start.x + pos.x, edge.start.y + pos.y },
                { edge.end.x   + pos.x, edge.end.y   + pos.y },
                edge.thickness,
                edge.color);
        }
    }

    for (Entity bullet_id : r.view<Bullet, Transform>()) {
        const auto& bullet = r.get<Bullet>(bullet_id);
        const auto& transform = r.get<Transform>(bullet_id);
        Vector2 pos = transform.position;

        DrawLineEx({ bullet.bullet.start.x + pos.x, bullet.bullet.start.y + pos.y },
                   { bullet.bullet.end.x   + pos.x, bullet.bullet.end.y   + pos.y, },
                   bullet.bullet.thickness,
                   bullet.bullet.color );
    }

    for (Entity particle_id : r.view<Particle, Transform>()) {
        const auto& particle = r.get<Particle>(particle_id);
        const auto& particle_transform = r.get<Transform>(particle_id);

        DrawCircleGradient(particle_transform.position, particle.radius, particle.color, BLACK);
    }

    float corner = .1f;
    Vector2 asteroids_remaining_hud = { SCREEN_WIDTH - SCREEN_WIDTH * corner, SCREEN_HEIGHT - SCREEN_HEIGHT * corner };
    std::string remaining = std::to_string(gs.remaining_asteroids);
    DrawTextEx(GetFontDefault(), remaining.c_str(), asteroids_remaining_hud, 32.f, 1.f, MY_GOLD);
}

inline void weapons_fire(Registry& r) {
    float bullet_speed = 500.f;
    float bullet_offset_from_ship = 15.f;
    float bullet_length = 11.f;
    float bullet_age = 0.f;
    float bullet_lifespan = .75f;
    Vector2 bullet_start = Vector2{ 0.f, 0.f };

    for (Entity e : r.view<PlayerInput, PolygonShip, Transform, Weapon>()) {
        const auto& ship = r.get<PolygonShip>(e);
        const auto& pt = r.get<Transform>(e);
        auto& input = r.get<PlayerInput>(e);
        auto& weapon = r.get<Weapon>(e);

        if (input.shoot && weapon.ready) {
            weapon.cooldown_timer += (1.f / weapon.max_ammo) * weapon.cooldown;
            if (weapon.cooldown_timer >= weapon.cooldown) { weapon.ready = false; }

            input.shoot = false;

            Vector2 bullet_end = Vector2{
                cosf(pt.orientation - PI/2.f) * bullet_length,
                sinf(pt.orientation - PI/2.f) * bullet_length };

            Entity new_bullet = r.create();
            r.add(new_bullet, Bullet{
                Line{ bullet_start, bullet_end, ORANGE, 2.5f },
                bullet_speed,
                bullet_age,
                bullet_lifespan });
            r.add(new_bullet, Transform{
                Vector2{
                    cosf(pt.orientation - PI/2.f) * (bullet_offset_from_ship + (bullet_length / 2.f)) + pt.position.x,
                    sinf(pt.orientation - PI/2.f) * (bullet_offset_from_ship + (bullet_length / 2.f)) + pt.position.y },
                Vector2{
                    cosf(pt.orientation - PI/2.f) * bullet_speed,
                    sinf(pt.orientation - PI/2.f) * bullet_speed },
                0.f,
                0.f,
                1.f,
                0.f });
        }

        if (weapon.cooldown_timer >  0.f) { weapon.cooldown_timer -= GetFrameTime(); }
        if (weapon.cooldown_timer <= 0.f) { weapon.ready = true; }
    }

    std::vector<Entity> dead_bullets;
    for (Entity bullet_id : r.view<Bullet>()) {
        auto& bullet = r.get<Bullet>(bullet_id);
        bullet.age += GetFrameTime();
        if (bullet.age >= bullet.lifespan) { r.add(bullet_id, Dead{}); }
    }
}

} // end namespace
