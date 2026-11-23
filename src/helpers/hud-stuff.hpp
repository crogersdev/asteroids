#pragma once

#include "../components.hpp"
#include "../constants.hpp"
#include "../entities.hpp"
#include "game-state.hpp"

namespace crogersdev {

inline void create_game_huds(Registry& r, std::shared_ptr<Assets> assets) {
    float hud_scale_factor = .667f;
    Vector2 hud_dimensions = { 90.f, 25.f };
    float hud_outline_thickness = 2.f;
    float hud_padding = 4.f;
    float hud_roundedness = 8.f;
    float hud_position_y = SCREEN_HEIGHT * .95f;

    Entity remaining_lives_hud_id = r.create();
    r.add(remaining_lives_hud_id, HUD{});
    r.add(remaining_lives_hud_id, crogersdev::Transform{{ SCREEN_WIDTH * .725f, hud_position_y }});
    r.add(remaining_lives_hud_id, HUDRect{
        HUDText{},
        Rectangle{ 0.f, 0.f, hud_dimensions.x, hud_dimensions.y },
        hud_padding,
        hud_roundedness,
        hud_outline_thickness,
        WHITE
    });
    r.add(remaining_lives_hud_id, Polygon{{
        Line{ Vector2Scale({ player_polygon_shape[0].start.x, player_polygon_shape[0].start.y }, hud_scale_factor), Vector2Scale({ player_polygon_shape[0].end.x, player_polygon_shape[0].end.y }, hud_scale_factor), MY_GOLD, 1.f },
        Line{ Vector2Scale({ player_polygon_shape[1].start.x, player_polygon_shape[1].start.y }, hud_scale_factor), Vector2Scale({ player_polygon_shape[1].end.x, player_polygon_shape[1].end.y }, hud_scale_factor), MY_GOLD, 1.f },
        Line{ Vector2Scale({ player_polygon_shape[2].start.x, player_polygon_shape[2].start.y }, hud_scale_factor), Vector2Scale({ player_polygon_shape[2].end.x, player_polygon_shape[2].end.y }, hud_scale_factor), MY_GOLD, 1.f },
        Line{ Vector2Scale({ player_polygon_shape[3].start.x, player_polygon_shape[3].start.y }, hud_scale_factor), Vector2Scale({ player_polygon_shape[3].end.x, player_polygon_shape[3].end.y }, hud_scale_factor), MY_GOLD, 1.f }}
    });

    Entity remaining_asteroids_hud_id = r.create();
    r.add(remaining_asteroids_hud_id, HUD{});
    r.add(remaining_asteroids_hud_id, crogersdev::Transform{{ SCREEN_WIDTH * .875f , hud_position_y }});
    r.add(remaining_asteroids_hud_id, HUDRect{
        HUDText{},
        Rectangle{ 0.f, 0.f, hud_dimensions.x, hud_dimensions.y },
        hud_padding,
        hud_roundedness,
        hud_outline_thickness,
        WHITE
    });
    r.add(remaining_asteroids_hud_id, HUDBar{ 
        1.f,
        Rectangle{ 0.f, 0.f, hud_dimensions.x - 2.f*hud_padding, hud_dimensions.y - 2.f*hud_padding },
        MY_GOLD
    });

    Entity score_hud_id = r.create();
    r.add(score_hud_id, HUD{});
    r.add(score_hud_id, crogersdev::Transform({ SCREEN_WIDTH * .575f }));
    r.add(score_hud_id, HUDRect{
        HUDText{ 20.f, assets->menu_title_font, MY_GOLD, assets->title_font_shader },
        Rectangle{ 0.f, 0.f, hud_dimensions.x, hud_dimensions.y },
        hud_padding,
        hud_roundedness,
        hud_outline_thickness,
        WHITE
    });

}

}