#pragma once
 
#include "../constants.hpp"

#include <iostream>
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
    int remaining_asteroids;

    Entity player_id;

    menu_options_t menu_selected_option;
    state_t current_state;

    GameState(Entity pid)
    : lives{},
      score{},
      remaining_asteroids{},
      level{},
      player_id{pid},
      menu_selected_option(menu_options_t::START),
      current_state(state_t::MENU) {
        lives = 5;
        level = 0;
        score = 0;
        remaining_asteroids = asteroids_per_level[0];
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
};

} // end namespace