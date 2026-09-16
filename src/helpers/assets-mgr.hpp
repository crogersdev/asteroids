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
    Font menu_title_font;
    Font menu_option_font;
    Shader title_font_shader;
    Timer game_start_animation_timer;

    Assets()
    : game_start_animation_timer(0.f, timer_game_start_animation_max),
      title_font_shader(LoadShader(0, "resources/shaders/glsl330/sdf.fs")),
      // NOTE: the relative path here is relative to where we build
      //       and not where the file lives relative to this file's
      //       location on disk
      menu_title_font(LoadFontEx("../assets/BadMofo.ttf", 512.f, nullptr, 0)),
      menu_option_font(LoadFontEx("../assets/cubic.ttf", 48.f, nullptr, 0))
    {
        SetTextureFilter(menu_title_font.texture, TEXTURE_FILTER_BILINEAR);
    };

    ~Assets() {
        UnloadFont(menu_option_font);
        UnloadFont(menu_title_font);
        UnloadShader(title_font_shader);
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