#pragma once

#include "mat4.hh"
#include "maths.hh"
#include "transform.hh"

class camera
{
public:
    camera() = default;

    float get_fov() const;
    void set_fov(float fov);

    transform& get_transform();

    // View matrix: inverse of the camera's model matrix
    mat4 view_matrix() const;

private:
    transform trans_;
    float fov_; // vertical field of view
};
