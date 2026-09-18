#include "scene_object.hh"

scene_object::scene_object(const mesh* mesh, const texture* tex, const transform& trans, shader_fn shader)
    : mesh_(mesh)
    , tex_(tex)
    , trans_(trans)
    , shader_(shader)
{}

const mesh* scene_object::get_mesh() const
{
    return mesh_;
}

const texture* scene_object::get_texture() const
{
    return tex_;
}

transform& scene_object::get_transform()
{
    return trans_;
}

const transform& scene_object::get_transform() const
{
    return trans_;
}

const shader_fn& scene_object::get_shader() const
{
    return shader_;
}

void scene_object::set_shader(shader_fn shader)
{
    shader_ = shader;
}
