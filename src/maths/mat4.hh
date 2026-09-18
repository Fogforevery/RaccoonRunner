#pragma once

#include <cmath>

#include "vec3.hh"

struct mat4
{
    float m[4][4] = {};

    static mat4 identity();
    static mat4 translation(float tx, float ty, float tz);
    static mat4 scale(float sx, float sy, float sz);
    static mat4 rotation_x(float a);
    static mat4 rotation_y(float a);
    static mat4 rotation_z(float a);

    mat4 dot(const mat4& o) const;

    // Transform a point (w=1) -> applies translation
    vec3 transform_point(const vec3& v) const;

    // Transform a direction
    vec3 transform_dir(const vec3& v) const;

    // Inverse for pure rotation + translation matrices (no scale)
    // https://en.wikipedia.org/wiki/Rigid_transformation#Formal_definition
    mat4 inverse_rigid() const;
};
