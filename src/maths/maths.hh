#pragma once

#include <cmath>

class maths
{
public:
    static constexpr float MY_PI = 3.14159265358979323846f;

    static float to_radians(float degrees);
    static float clamp(float v, float lo, float hi);
    static float clamp_unit(float v);
    static float lerp(float a, float b, float t);
    static float remap_unit(float v, float lo, float hi);
};
