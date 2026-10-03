#pragma once

#include "../entities.hpp"
#include "../components.hpp"
#include "../constants.hpp"
#include "../helpers/math-stuff.hpp"

namespace crogersdev {

inline void generate_particles(Registry& registry, Vector2 position, uint32_t particle_num = particle_max) {
    for (int p = 0; p < particle_num; p++) {
        auto particle_theta = my_rng(0.f, 2.f * PI, Dist::Uniform);
        auto particle_speed = my_rng(250.f, 50.f, Dist::Normal);
        auto particle_radius = my_rng(3.5f, .5f, Dist::Normal);
        auto particle_lifespan = my_rng(.1f, .75f, Dist::Normal);

        Entity particle = registry.create();
        registry.add(particle, Particle{
            particle_age,
            particle_lifespan,
            particle_radius,
            WHITE });
        registry.add(particle, Transform{
            position,
            { cos(particle_theta) * particle_speed, sin(particle_theta) * particle_speed },
            0.f,
            particle_drag,
            0.f });
    }
}

} // end namespace