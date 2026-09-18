#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "maths/camera.hh"
#include "maths/transform.hh"
#include "mesh.hh"
#include "render_target.hh"
#include "shader.hh"
#include "texture.hh"

using shader_fn = std::function<vec3(const shader_input&, const vec3&)>;

class scene_object
{
public:
    scene_object() = default;
    scene_object(const mesh* mesh, const texture* tex, const transform& trans, shader_fn shader);

    const mesh* get_mesh() const;
    const texture* get_texture() const;
    transform& get_transform(); // TODO will be removed
    const transform& get_transform() const;
    const shader_fn& get_shader() const;
    void set_shader(shader_fn shader);

private:
    const mesh* mesh_;
    const texture* tex_;
    transform trans_;
    shader_fn shader_;
};
