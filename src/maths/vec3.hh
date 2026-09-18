#pragma once

struct vec3
{
    float x, y, z;

    vec3() = default;
    vec3(float x, float y, float z);

    friend vec3 operator*(const vec3& v, float k);
    friend vec3 operator/(const vec3& v, float k);

    friend vec3 operator+(const vec3& a, const vec3& b);
    friend vec3 operator-(const vec3& a, const vec3& b);
    friend vec3 operator*(const vec3& a, const vec3& b);
    friend vec3 operator/(const vec3& a, const vec3& b);

    vec3 operator-() const;

    vec3& operator+=(const vec3& o);
    vec3& operator-=(const vec3& o);
    vec3& operator*=(const vec3& o);
    vec3& operator/=(const vec3& o);

    float operator[](int i) const;
    float& operator[](int i);

    static float dot(const vec3& a, const vec3& b);
    static vec3 cross(const vec3& a, const vec3& b);
    static float length(const vec3& v);
    static vec3 normalize(const vec3& v);
    static vec3 clamp(const vec3& v, float lo, float hi);
    static vec3 lerp(const vec3& a, const vec3& b, float t);
};
