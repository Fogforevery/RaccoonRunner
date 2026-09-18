#include "maths.hh"

#include <algorithm>
#include <cmath>

float maths::to_radians(float degrees)
{
    return degrees * (MY_PI / 180.0f);
}

float maths::clamp(float v, float lo, float hi)
{
    return std::clamp(v, lo, hi);
}

float maths::clamp_unit(float v)
{
    return clamp(v, 0.0f, 1.0f);
}

float maths::lerp(float a, float b, float t)
{
    return a + (b - a) * clamp_unit(t);
}

float maths::remap_unit(float v, float lo, float hi)
{
    return clamp_unit((v - lo) / (hi - lo));
}
