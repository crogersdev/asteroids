#pragma once

#include <tuple>

#include <raylib.h>
#include <raymath.h>

namespace crogersdev {

inline std::tuple<Vector2, Vector2> compute_collision_velocities(Vector2 va, Vector2 vb, Vector2 pa, Vector2 pb, float ma, float mb) {
    Vector2 n = Vector2Normalize(Vector2Subtract(pa, pb));
    Vector2 t = Vector2Rotate(n, PI / 2.f);
    float na = Vector2DotProduct(va, n);
    float nb = Vector2DotProduct(vb, n);
    float ta = Vector2DotProduct(va, t);
    float tb = Vector2DotProduct(vb, t);
    float mt = ma + mb;

    float na_prime = ((ma - mb) / mt) * na + (2 * mb / mt) * nb;
    float nb_prime = ((mb - ma) / mt) * nb + (2 * ma / mt) * na;

    return std::tuple<Vector2, Vector2> { 
        Vector2Add(Vector2Scale(n, na_prime), Vector2Scale(t, ta)),
        Vector2Add(Vector2Scale(n, nb_prime), Vector2Scale(t, tb))
    };
}

} // end namespace