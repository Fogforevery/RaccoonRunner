#include "vec2.hh"

#include "maths.hh"

vec2::vec2(float x, float y)
    : x(x)
    , y(y)
{}

vec2 operator*(const vec2& v, float k)
{
    return vec2(v.x * k, v.y * k);
}

vec2 operator/(const vec2& v, float k)
{
    return vec2(v.x / k, v.y / k);
}

vec2 operator+(const vec2& a, const vec2& b)
{
    return vec2(a.x + b.x, a.y + b.y);
}

vec2 operator-(const vec2& a, const vec2& b)
{
    return vec2(a.x - b.x, a.y - b.y);
}

vec2 operator*(const vec2& a, const vec2& b)
{
    return vec2(a.x * b.x, a.y * b.y);
}

vec2 operator/(const vec2& a, const vec2& b)
{
    return vec2(a.x / b.x, a.y / b.y);
}

vec2 vec2::operator-()
{
    return vec2(-x, -y);
}

vec2& vec2::operator+=(const vec2& o)
{
    x += o.x;
    y += o.y;
    return *this;
}

vec2& vec2::operator-=(const vec2& o)
{
    x -= o.x;
    y -= o.y;
    return *this;
}

vec2& vec2::operator*=(const vec2& o)
{
    x *= o.x;
    y *= o.y;
    return *this;
}

float vec2::dot(const vec2& a, const vec2& b)
{
    return a.x * b.x + a.y * b.y;
}

float vec2::length(const vec2& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

vec2 vec2::normalize(const vec2& v)
{
    return v / length(v);
}

vec2 vec2::lerp(const vec2& a, const vec2& b, float t)
{
    return a + (b - a) * t;
}
