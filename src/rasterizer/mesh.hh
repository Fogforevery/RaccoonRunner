#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "maths/vec2.hh"
#include "maths/vec3.hh"
#include "texture.hh"

struct material_props
{
    float ns;
    vec3 ka;
    vec3 kd;
    vec3 ks;
    vec3 ke;
    float ni;
    float d;
    unsigned illum;

    const texture* map_kd = nullptr;
    vec2 tex_scale{ 1.0f, 1.0f };
    vec2 tex_offset{ 0.0f, 0.0f };
};

struct vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
};

struct triangle
{
    vertex vertices[3];
    const material_props* material = nullptr;
};

struct rast_vertex
{
    vec3 world_pos; // position in world space
    vec3 view_pos; // position in view space
    vec2 screen_pos; // projected pixel coordinate, filled by draw_triangle
    vec3 world_normal; // normal in world space
    vec2 uv; // texture coordinate
};

class mesh
{
public:
    mesh() = default;

    const std::vector<triangle>& get_triangles() const;
    void add_triangle(const triangle& tri);

    void set_materials(std::unordered_map<std::string, material_props> materials);
    const std::unordered_map<std::string, material_props>& get_materials() const;
    const material_props* find_material(const std::string& name) const;

    static mesh make_cube();

    void set_textures(std::unordered_map<std::string, std::unique_ptr<texture>> textures);

private:
    // Flat list, every three gives one triangle
    std::vector<triangle> triangles_;
    // Give materials ownership to this struct so that lifetime is longer than OBJ/MTL loading
    std::unordered_map<std::string, material_props> materials_;
    std::unordered_map<std::string, std::unique_ptr<texture>> textures_;
};
