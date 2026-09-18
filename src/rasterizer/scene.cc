#include "scene.hh"

std::vector<std::shared_ptr<scene_object>>& scene::get_objects()
{
    return objects_;
}

const std::vector<std::shared_ptr<scene_object>>& scene::get_objects() const
{
    return objects_;
}

camera& scene::get_camera()
{
    return cam_;
}

const camera& scene::get_camera() const
{
    return cam_;
}

dir_light& scene::get_light()
{
    return light_;
}

const dir_light& scene::get_light() const
{
    return light_;
}

std::shared_ptr<scene_object> scene::add(const mesh* mesh, const texture* tex, shader_fn shader)
{
    objects_.push_back(std::make_shared<scene_object>(scene_object{ mesh, tex, {}, shader }));

    return objects_.back();
}
