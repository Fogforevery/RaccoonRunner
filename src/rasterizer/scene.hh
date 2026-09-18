#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "maths/camera.hh"
#include "maths/transform.hh"
#include "mesh.hh"
#include "render_target.hh"
#include "scene_object.hh"
#include "shader.hh"
#include "texture.hh"

class scene
{
public:
    scene() = default;

    std::vector<std::shared_ptr<scene_object>>& get_objects();
    const std::vector<std::shared_ptr<scene_object>>& get_objects() const;

    camera& get_camera();
    const camera& get_camera() const;

    dir_light& get_light();
    const dir_light& get_light() const;

    std::shared_ptr<scene_object> add(const mesh* mesh, const texture* tex, shader_fn shader);

private:
    std::vector<std::shared_ptr<scene_object>> objects_;
    camera cam_;
    dir_light light_;
};
