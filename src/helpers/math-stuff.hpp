#pragma once

#include <random>
#include <type_traits>
#include <raymath.h>

namespace crogersdev {

enum class Dist { Normal, Uniform };

inline std::mt19937& get_generator() {
    thread_local std::mt19937 gen{ std::random_device{}() };
    return gen;
}

template <typename T>
inline T my_rng(T a, T b, Dist type) {
    auto& gen = get_generator();

    if (type == Dist::Uniform) {
        if constexpr (std::is_floating_point_v<T>) { return std::uniform_real_distribution<T>(a, b)(gen); }
        else { return std::uniform_int_distribution<T>(a, b)(gen); }
    }

    if constexpr (std::is_floating_point_v<T>) { return std::normal_distribution<T>(a,b)(gen); }
    else {
        std::normal_distribution<double> dist(static_cast<double>(a), static_cast<double>(b));
        return static_cast<T>(std::round(dist(gen)));
    }
}

template <typename T>
inline T normalize(T x, T min, T max) { return (x - min) / (max - min); };

Vector2 rotate_vector(Vector2 point, Vector2 pivot, float angleRadians) {
    // Step 1: Translate point back to the origin relative to the pivot
    Vector2 translatedPoint = Vector2Subtract(point, pivot);
    
    // Step 2: Rotate the origin-centered vector
    // NOTE: raymath's Vector2Rotate() expects the angle in RADIANS
    Vector2 rotatedPoint = Vector2Rotate(translatedPoint, angleRadians);
    
    // Step 3: Translate the point back to its original pivot space
    return Vector2Add(rotatedPoint, pivot);
}
} // end namespace