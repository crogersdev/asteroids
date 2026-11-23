#pragma once

#include "../components.hpp"
#include "../constants.hpp"

namespace crogersdev {

//inline void shield_color_update(Registry& r, GameState gs, bool reset = false) {
inline void entity_color_update(Entity entity_id) {
    /*
    Entity shield_id = r.view<Shield>().front();
    auto& shield = r.get<Shield>(shield_id);

    if (reset) { shield.energy_remaining = shield_max_energy; }

    if (shield.energy_remaining > 0.f) {
        auto palette = gs.shield_palette;
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
        */
}


}