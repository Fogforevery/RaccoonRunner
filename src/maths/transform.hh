#pragma once

#include "mat4.hh"
#include "maths.hh"

class transform
{
public:
    transform();

    const vec3& get_position() const;
    float get_position_x() const;
    float get_position_y() const;
    float get_position_z() const;

    void set_position(const vec3& position);
    void set_position_x(float rot_x);
    void set_position_y(float rot_y);
    void set_position_z(float rot_z);

    const vec3& get_rotation() const;
    float get_rotation_x() const;
    float get_rotation_y() const;
    float get_rotation_z() const;

    void set_rotation(const vec3& rotation);
    void set_rotation_x(float rot_x);
    void set_rotation_y(float rot_y);
    void set_rotation_z(float rot_z);

    const vec3& get_scale() const;
    void set_scale(const vec3& scale);

    // Build model matrix: scale -> rot_x -> rot_y -> rot_x -> translate
    mat4 model_matrix() const;

private:
    vec3 position_;
    vec3 rotation_; // pitch, yaw, roll (in radians)
    vec3 scale_;
};
