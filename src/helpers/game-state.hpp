#pragma once
 
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

std::ostream& operator<<(std::ostream& o, const menu_options_t m) {
    switch(m) {
        case menu_options_t::SETTINGS: o << "SETTINGS"; break;
        case menu_options_t::START:    o << "START";    break;
        case menu_options_t::QUIT:     o << "QUIT";     break;
    }
    return o;
}

enum class state_t {
    DYING,
    GAME_OVER,
    MENU,
    NEW_GAME, 
    PLAYING,
    PAUSED,
    QUITTING,
    UPDATE_SETTINGS
};

struct GameState {
    int lives;
    int score;
    int starting_asteroid_count;

    menu_options_t menu_selected_option;
    state_t current_state;

    GameState()
    : lives(5),
      score(0),
      starting_asteroid_count(4),
      menu_selected_option(menu_options_t::START),
      current_state(state_t::MENU) { };

    GameState(int l, int s, int a)
    : lives(l),
      score(s),
      starting_asteroid_count(a),
      menu_selected_option(menu_options_t::START),
      current_state(state_t::MENU) { };

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