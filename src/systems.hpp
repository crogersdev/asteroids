#pragma once

#include "entities.hpp"

#include <raylib.h>

namespace crogersdev {

inline void renderSystem(Player player) {
    // note: raylib's approach is to have global access to raylib managed
    //       resources.  in this case, that's the window to which we render
    //       and draw all our stuff.  my initial instinct was to have to pass
    //       in a reference to the window and use that reference
    //       to invoke any DrawFoo type methods but we don't have to do that.
    DrawTextureRec(
        player.sprite,
        source,
        position vec,
        tint color
    );

}

}
