#pragma once

#include "../constants.hpp"

#include <raylib.h>

namespace crogersdev {

struct Timer {
    float current_time;
    float max_time;

    Timer() : current_time(0.f), max_time(0.f) {};
    Timer(float c, float m) : current_time(c), max_time(m) {};
};

struct Assets {
    Timer game_start_animation_timer;

    Font menu_title_font;
    Font menu_option_font;

    Assets()
    : game_start_animation_timer(0.f, timer_game_start_animation_max),
      menu_title_font(LoadFontEx("../../assets/BadMofo.ttf", 150.f, nullptr, 0)),
      menu_option_font(LoadFontEx("../../assets/cubic.ttf", 48.f, nullptr, 0)) { };

    ~Assets() {
        UnloadFont(menu_option_font);
        UnloadFont(menu_title_font);
    }

    // NOTE: explicitly prevent copy ctor; raylib's underlying Font struct
    //       uses a Texture2D object which manually allocates memory.
    //       if we don't prevent copy ctor's then we'll have multiple
    //       objects pointing at the same spot in memory and have double
    //       free errors on destruction.
    Assets(const Assets&) = delete;
    Assets operator=(const Assets&) = delete;
};

} // end namespace