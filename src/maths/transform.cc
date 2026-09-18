#include "transform.hh"

transform::transform()
    : position_{ 0.0f, 0.0f, 0.0f }
    , rotation_{ 0.0f, 0.0f, 0.0f }
    , scale_{ 1.0f, 1.0f, 1.0f }
{}

const vec3& transform::get_position() const
{
    return position_;
}
float transform::get_position_x() const
{
    return position_.x;
}
float transform::get_position_y() const
{
    return position_.y;
}
float transform::get_position_z() const
{
    return position_.z;
}

void transform::set_position(const vec3& position)
{
    position_ = position;
}
void transform::set_position_x(float pos_x)
{
    position_.x = pos_x;
}
void transform::set_position_y(float pos_y)
{
    position_.y = pos_y;
}
void transform::set_position_z(float pos_z)
{
    position_.z = pos_z;
}

const vec3& transform::get_rotation() const
{
    return rotation_;
}
float transform::get_rotation_x() const
{
    return rotation_.x;
}
float transform::get_rotation_y() const
{
    return rotation_.y;
}
float transform::get_rotation_z() const
{
    return rotation_.z;
}

void transform::set_rotation(const vec3& rotation)
{
    rotation_ = rotation;
}
void transform::set_rotation_x(float rot_x)
{
    rotation_.x = rot_x;
}
void transform::set_rotation_y(float rot_y)
{
    rotation_.y = rot_y;
}
void transform::set_rotation_z(float rot_z)
{
    rotation_.z = rot_z;
}

const vec3& transform::get_scale() const
{
    return scale_;
}
void transform::set_scale(const vec3& scale)
{
    scale_ = scale;
}

mat4 transform::model_matrix() const
{
    mat4 s = mat4::scale(scale_.x, scale_.y, scale_.z);
    mat4 rot_x = mat4::rotation_x(rotation_.x);
    mat4 rot_y = mat4::rotation_y(rotation_.y);
    mat4 rot_z = mat4::rotation_z(rotation_.z);
    mat4 t = mat4::translation(position_.x, position_.y, position_.z);

    return t.dot(rot_z.dot(rot_y.dot(rot_x.dot(s)))); // lol
}
