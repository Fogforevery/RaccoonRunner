#include "camera.hh"

float camera::get_fov() const
{
    return fov_;
}

void camera::set_fov(float fov)
{
    fov_ = fov;
}

transform& camera::get_transform()
{
    return trans_;
}

mat4 camera::view_matrix() const
{
    mat4 rot_x = mat4::rotation_x(trans_.get_rotation().x);
    mat4 rot_y = mat4::rotation_y(trans_.get_rotation().y);
    mat4 rot_z = mat4::rotation_z(trans_.get_rotation().z);
    mat4 t = mat4::translation(trans_.get_position().x, trans_.get_position().y, trans_.get_position().z);
    mat4 cam_matrix = t.dot(rot_z.dot(rot_y.dot(rot_x)));

    return cam_matrix.inverse_rigid();
}
