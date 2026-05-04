#pragma once

#include "asteroid-generator.hpp"
#include "../components.hpp"
#include "../constants.hpp"
#include "../entities.hpp"

#include <cmath>
#include <iostream>
#include <random>

#include <raylib.h>

namespace crogersdev {

inline void clear_player_inputs(Registry& registry) {
    for (Entity& player_id : registry.view<PlayerInput>()) {
        auto& player = registry.get<PlayerInput>(player_id);
        player.thrust = false;
        player.shoot = false;
        player.rotate_left = false;
        player.rotate_right = false;
    }
}

inline void collision_system(Registry& registry) {
    std::vector<Entity> dead_asteroids;
    std::vector<Entity> dead_bullets;
    std::vector<Entity> dead_particles;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> speed(250.f, 50.f);
    std::normal_distribution<float> radius(3.5f, .5f);
    std::normal_distribution<float> lifespan(.1f, .75f);
    std::normal_distribution<float> breakoff_speed(1.f, .66f);
    std::uniform_real_distribution<float> angle(0.f, 2.f * PI);

    float particle_age = 0.f;
    float particle_drag = 0.999f;

    // TODO: If you're going to make bullets long lines, you'll need
    // to track collision through leading edge or possibly all points along the line
    // in order to preserve things like an asteroid crossing that line
    for (Entity& bullet : registry.view<Bullet, Transform>()) {
        auto& bullet_transform = registry.get<Transform>(bullet);

        for (Entity& asteroid : registry.view<Asteroid, Size, Transform>()) {
            auto& asteroid_transform = registry.get<Transform>(asteroid);
            auto& asteroid_size = registry.get<Size>(asteroid);

            auto asteroid_collision_radius = asteroid_size.radius * asteroid_size.size;

            auto bullet_distance_to_asteroid =
                pow(bullet_transform.position.x - asteroid_transform.position.x, 2) +
                pow(bullet_transform.position.y - asteroid_transform.position.y, 2);

            if (bullet_distance_to_asteroid <= pow(asteroid_collision_radius, 2)) {
                for (int p = 0; p < 50; p++) {
                    auto particle_theta = angle(gen);
                    auto particle_speed = speed(gen);
                    auto particle_radius = radius(gen);
                    auto particle_lifespan = lifespan(gen);

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
                        particle_drag });
                }

                if (asteroid_size.size > 1) {
                    auto parent_speed = sqrt(pow(asteroid_transform.velocity.x, 2.f) + pow(asteroid_transform.velocity.y, 2.f));
                    int child_asteroids = 2;
                    for (int i = 0; i < child_asteroids; i++) {
                        Entity new_asteroid = registry.create();
                        auto new_theta = angle(gen);
                        auto new_speed = breakoff_speed(gen) * parent_speed;
                        registry.add(new_asteroid, Size{ asteroid_size.radius, asteroid_size.size-1 });
                        registry.add(new_asteroid, Transform{
                            asteroid_transform.position,
                            { cos(new_theta) * new_speed, sin(new_theta) * new_speed },
                            0.f,
                            1.f });
                        registry.add(new_asteroid, Asteroid{ generate_asteroid(asteroid_size.size-1, asteroid_size.radius, RED, 1.25f) });
                    }
                }

                dead_asteroids.push_back(asteroid);
                dead_bullets.push_back(bullet);
            }
        }
    }

    for (Entity& particle : registry.view<Particle>()) {
        auto& particle_info = registry.get<Particle>(particle);

        particle_info.age += GetFrameTime();

        if (particle_info.age >= particle_info.lifespan) {
            dead_particles.push_back(particle);
        }
    }

    for (auto& b : dead_bullets) {
        registry.destroy(b);
    }
    for (auto& a : dead_asteroids) {
        registry.destroy(a);
    }
    for (auto& p : dead_particles) {
        registry.destroy(p);
    }
}

inline void draw_debug_info() {
    DrawCircle(GetScreenWidth() / 2.f, GetScreenHeight() / 2.f, 2.f, WHITE);
}

inline void movement_update_system(Registry& registry) {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();

    for (Entity& e : registry.view<Transform, PlayerInput, PolygonShip>()) {
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
    }

    for (Entity& e : registry.view<Transform>()) {
        auto& transform = registry.get<Transform>(e);

        Vector2 dt_offset = { transform.velocity.x * transform.drag * GetFrameTime(),
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

inline void player_input_system(Registry& registry) {
    for (Entity& player_id : registry.view<PlayerInput>()) {
        auto& player = registry.get<PlayerInput>(player_id);

        if (IsKeyDown(KEY_W))        { player.thrust = true; }
        if (IsKeyDown(KEY_A))        { player.rotate_left = true; }
        if (IsKeyDown(KEY_S))        { }
        if (IsKeyDown(KEY_D))        { player.rotate_right = true; }
        if (IsKeyPressed(KEY_SPACE)) { player.shoot = true; }
    }
}

inline void sound_system(Registry& registry) {
}

inline void render_system(Registry& registry) {
    // note: raylib's approach is to have global access to raylib managed
    //       resources.  in this case, that's the window to which we render
    //       and draw all our stuff.  that means we can just straight up
    //       call DrawLineEx without any problems or concerns

    for (Entity& e : registry.view<PolygonShip, Transform>()) {
        auto& ship = registry.get<PolygonShip>(e);
        auto& transform = registry.get<Transform>(e);
        Vector2 pos = transform.position;

        Vector2 start = {}, end = {};
        for (auto& ship_edge : ship.lines) {
            start.x = ship_edge.start.x + pos.x;
            start.y = ship_edge.start.y + pos.y;
            end.x = ship_edge.end.x + pos.x;
            end.y = ship_edge.end.y + pos.y;

            DrawLineEx(start, end, ship_edge.thickness, ship_edge.color);
        }
    }

    for (Entity& e : registry.view<Asteroid, Transform>()) {
        auto& asteroid = registry.get<Asteroid>(e);
        auto& transform = registry.get<Transform>(e);
        Vector2 pos = transform.position;

        for (auto& asteroid_edge : asteroid.lines) {
            DrawLineEx({ asteroid_edge.start.x + pos.x, asteroid_edge.start.y + pos.y },
                       { asteroid_edge.end.x   + pos.x, asteroid_edge.end.y   + pos.y },
                       asteroid_edge.thickness,
                       asteroid_edge.color);
        }
    }

    for (Entity& e : registry.view<Bullet, Transform>()) {
        auto& bullet = registry.get<Bullet>(e);
        auto& transform = registry.get<Transform>(e);
        Vector2 pos = transform.position;

        DrawLineEx({ bullet.bullet.start.x + pos.x, bullet.bullet.start.y + pos.y },
                   { bullet.bullet.end.x   + pos.x, bullet.bullet.end.y   + pos.y, },
                   bullet.bullet.thickness,
                   bullet.bullet.color );
    }

    for (Entity& e : registry.view<Particle, Transform>()) {
        auto& particle = registry.get<Particle>(e);
        auto& particle_transform = registry.get<Transform>(e);
        Vector2 pos = particle_transform.position;

        DrawCircleGradient(pos.x, pos.y, particle.radius, particle.color, BLACK);
    }
}

inline void weapon_system(Registry& registry) {
    float bullet_speed = 500.f;
    float bullet_offset_from_ship = 15.f;
    float bullet_length = 11.f;
    float bullet_age = 0.f;
    float bullet_lifespan = .75f;
    Vector2 bullet_start = Vector2{ 0.f, 0.f };

    for (Entity& e : registry.view<PlayerInput, PolygonShip, Transform, Weapon>()) {
        auto& input = registry.get<PlayerInput>(e);
        auto& ship = registry.get<PolygonShip>(e);
        auto& player_transform = registry.get<Transform>(e);
        auto& weapon = registry.get<Weapon>(e);

        if (input.shoot && weapon.ready) {

            weapon.cooldown_timer += (1.f / weapon.max_ammo) * weapon.cooldown;
            if (weapon.cooldown_timer >= weapon.cooldown) { weapon.ready = false; }

            input.shoot = false;

            auto ship_theta = atan2(ship.orientation.y, ship.orientation.x);
            Vector2 bullet_end = Vector2{
                cos(ship_theta) * bullet_length,
                sin(ship_theta) * bullet_length };

            Entity bullet = registry.create();
            registry.add(bullet, Bullet{
                Line{ bullet_start, bullet_end, ORANGE, 2.5f },
                bullet_speed,
                bullet_age,
                bullet_lifespan });
            registry.add(bullet, Transform{
                Vector2{
                    cos(ship_theta) * (bullet_offset_from_ship + (bullet_length / 2.f)) + player_transform.position.x,
                    sin(ship_theta) * (bullet_offset_from_ship + (bullet_length / 2.f)) + player_transform.position.y },
                Vector2{
                    cos(ship_theta) * bullet_speed,
                    sin(ship_theta) * bullet_speed },
                0.f,
                1.f });
        }

        if (weapon.cooldown_timer >  0.f) { weapon.cooldown_timer -= GetFrameTime(); }
        if (weapon.cooldown_timer <= 0.f) { weapon.ready = true; }
    }

    std::vector<Entity> dead_bullets;
    for (Entity& e : registry.view<Bullet>()) {
        auto& bullet = registry.get<Bullet>(e);

        bullet.age += GetFrameTime();

        if (bullet.age >= bullet.lifespan) {
            dead_bullets.push_back(e);
        }
    }

    for (auto& b : dead_bullets) {
        registry.destroy(b);
    }
}

} // end namespace
