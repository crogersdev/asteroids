#pragma once
 
#include "../constants.hpp"

#include <iostream>
#include <map>
#include <set>
#include <utility>

namespace crogersdev {

inline std::set<std::pair<Entity, Entity>> colliding_objects;

enum class menu_options_t {
    START,
    SETTINGS,
    QUIT
};

enum class state_t {
    DYING,
    GAME_OVER,
    LEVEL_CLEAR,
    LEVEL_START,
    MENU,
    NEW_GAME, 
    PLAYING,
    PAUSED,
    QUITTING,
    UPDATE_SETTINGS
};

struct GameState {
    int level;
    int lives;
    int score;
    std::array<GradientPalette, 3> hud_palettes;
    std::map<asteroid_size_t, uint32_t> remaining_asteroids;
    menu_options_t menu_selected_option;
    state_t current_state;
    Entity player_id;
    GradientPalette shield_palette;

    GameState(Entity pid)
    : level{},
      lives{},
      score{},
      hud_palettes{},
      remaining_asteroids{},
      menu_selected_option{},
      current_state{},
      player_id{pid},
      shield_palette{} {
        lives = 5;
        level = 0;
        score = 0;
        menu_selected_option = menu_options_t::START;
        current_state = state_t::MENU;
        hud_palettes = { green_yellow_red, neon_synth, solar_eclipse };
        shield_palette = neon_synth;
      };

    inline void nextMenuOption(menu_options_t& current) {
        switch (current) {
            case menu_options_t::START:    current = menu_options_t::SETTINGS;  break;
            case menu_options_t::SETTINGS: current = menu_options_t::QUIT;      break;
            case menu_options_t::QUIT:     current = menu_options_t::START;     break;
        }
    };

    inline void prevMenuOption(menu_options_t& current) {
        switch (current) {
            case menu_options_t::START:    current = menu_options_t::QUIT;     break;
            case menu_options_t::SETTINGS: current = menu_options_t::START;    break;
            case menu_options_t::QUIT:     current = menu_options_t::SETTINGS; break;
        }
    };

    inline void printRemainingAsteroids() {
        for (const auto& asteroid : remaining_asteroids) {
            std::cout << asteroid.first << "\t" << asteroid.second << "\n";
        }
    };
};

} // end namespace
