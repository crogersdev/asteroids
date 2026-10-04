#pragma once

#include "../entities.hpp"
#include "../components.hpp"
#include "../constants.hpp"
#include "../helpers/math-stuff.hpp"

namespace crogersdev {

inline void generate_particles(
    Registry& registry,
    Vector2 position,
    float orientation = 0.f,
    float scatter_cone_theta = PI,
    uint32_t particle_num = particle_max
    ) {
    for (int p = 0; p < particle_num; p++) {
        float particle_theta = my_rng(orientation - scatter_cone_theta, orientation + scatter_cone_theta, Dist::Uniform);
        float particle_speed = my_rng(250.f, 50.f, Dist::Normal);
        float particle_radius = my_rng(3.5f, .5f, Dist::Normal);
        float particle_lifespan = my_rng(.1f, .75f, Dist::Normal);

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