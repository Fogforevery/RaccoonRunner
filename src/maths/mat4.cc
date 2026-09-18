#include "mat4.hh"

mat4 mat4::identity()
{
    mat4 r;
    r.m[0][0] = r.m[1][1] = r.m[2][2] = r.m[3][3] = 1.f;
    return r;
}

mat4 mat4::translation(float tx, float ty, float tz)
{
    mat4 r = identity();
    r.m[3][0] = tx;
    r.m[3][1] = ty;
    r.m[3][2] = tz;
    return r;
}

mat4 mat4::scale(float sx, float sy, float sz)
{
    mat4 r = identity();
    r.m[0][0] = sx;
    r.m[1][1] = sy;
    r.m[2][2] = sz;
    return r;
}

mat4 mat4::rotation_x(float a)
{
    mat4 r = identity();
    r.m[1][1] = std::cos(a);
    r.m[2][1] = -std::sin(a);
    r.m[1][2] = std::sin(a);
    r.m[2][2] = std::cos(a);
    return r;
}

mat4 mat4::rotation_y(float a)
{
    mat4 r = identity();
    r.m[0][0] = std::cos(a);
    r.m[2][0] = std::sin(a);
    r.m[0][2] = -std::sin(a);
    r.m[2][2] = std::cos(a);
    return r;
}

mat4 mat4::rotation_z(float a)
{
    mat4 r = identity();
    r.m[0][0] = std::cos(a);
    r.m[1][0] = -std::sin(a);
    r.m[0][1] = std::sin(a);
    r.m[1][1] = std::cos(a);
    return r;
}

mat4 mat4::dot(const mat4& o) const
{
    mat4 r;

    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            float sum = 0;

            for (int k = 0; k < 4; k++)
                sum += m[k][row] * o.m[col][k];

            r.m[col][row] = sum;
        }
    }

    return r;
}

vec3 mat4::transform_point(const vec3& v) const
{
    float x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
    float y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
    float z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];
    float w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3];

    if (w != 0.f && w != 1.f)
    {
        x /= w;
        y /= w;
        z /= w;
    }

    return { x, y, z };
}

vec3 mat4::transform_dir(const vec3& v) const
{
    return { m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z, m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
             m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z };
}

mat4 mat4::inverse_rigid() const
{
    mat4 r;

    // Transpose
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            r.m[i][j] = m[j][i];

    // Invert the translation
    r.m[3][0] = -(r.m[0][0] * m[3][0] + r.m[1][0] * m[3][1] + r.m[2][0] * m[3][2]);
    r.m[3][1] = -(r.m[0][1] * m[3][0] + r.m[1][1] * m[3][1] + r.m[2][1] * m[3][2]);
    r.m[3][2] = -(r.m[0][2] * m[3][0] + r.m[1][2] * m[3][1] + r.m[2][2] * m[3][2]);
    r.m[3][3] = 1.f;

    return r;
}
