#pragma once

struct vec2
{
    float x, y;

    vec2() = default;
    vec2(float x, float y);

    friend vec2 operator*(const vec2& v, float k);
    friend vec2 operator/(const vec2& v, float k);

    friend vec2 operator+(const vec2& a, const vec2& b);
    friend vec2 operator-(const vec2& a, const vec2& b);
    friend vec2 operator*(const vec2& a, const vec2& b);
    friend vec2 operator/(const vec2& a, const vec2& b);

    vec2 operator-();

    vec2& operator+=(const vec2& o);
    vec2& operator-=(const vec2& o);
    vec2& operator*=(const vec2& o);

    static float dot(const vec2& a, const vec2& b);
    static float length(const vec2& v);
    static vec2 normalize(const vec2& v);
    static vec2 lerp(const vec2& a, const vec2& b, float t);
};
