#include "vec3.hh"

#include "maths.hh"

vec3::vec3(float x, float y, float z)
    : x(x)
    , y(y)
    , z(z)
{}

vec3 operator*(const vec3& v, float k)
{
    return vec3(v.x * k, v.y * k, v.z * k);
}

vec3 operator/(const vec3& v, float k)
{
    return vec3(v.x / k, v.y / k, v.z / k);
}

vec3 operator+(const vec3& a, const vec3& b)
{
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3 operator-(const vec3& a, const vec3& b)
{
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

vec3 operator*(const vec3& a, const vec3& b)
{
    return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

vec3 operator/(const vec3& a, const vec3& b)
{
    return vec3(a.x / b.x, a.y / b.y, a.z / b.z);
}

vec3 vec3::operator-() const
{
    return vec3(-x, -y, -z);
}

vec3& vec3::operator+=(const vec3& o)
{
    x += o.x;
    y += o.y;
    z += o.z;
    return *this;
}

vec3& vec3::operator-=(const vec3& o)
{
    x -= o.x;
    y -= o.y;
    z -= o.z;
    return *this;
}

vec3& vec3::operator*=(const vec3& o)
{
    x *= o.x;
    y *= o.y;
    z *= o.z;
    return *this;
}

vec3& vec3::operator/=(const vec3& o)
{
    x /= o.x;
    y /= o.y;
    z /= o.z;
    return *this;
}

float vec3::operator[](int i) const
{
    return (&x)[i];
}

float& vec3::operator[](int i)
{
    return (&x)[i];
}

float vec3::dot(const vec3& a, const vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 vec3::cross(const vec3& a, const vec3& b)
{
    return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float vec3::length(const vec3& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3 vec3::normalize(const vec3& v)
{
    return v / length(v);
}

vec3 vec3::clamp(const vec3& v, float lo, float hi)
{
    float x = maths::clamp(v.x, lo, hi);
    float y = maths::clamp(v.y, lo, hi);
    float z = maths::clamp(v.z, lo, hi);
    return vec3{ x, y, z };
}

vec3 vec3::lerp(const vec3& a, const vec3& b, float t)
{
    return a + (b - a) * t;
}
