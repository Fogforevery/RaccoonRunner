#pragma once

#include "maths/maths.hh"
#include "maths/vec2.hh"
#include "maths/vec3.hh"
#include "rasterizer/mesh.hh"
#include "rasterizer/texture.hh"

inline constexpr float BLENDER_AMBIENT_MAGIC_VALUE = 0.01;

struct shader_input
{
    vec2 uv; // perspective-correct texture coordinate
    vec3 world_normal; // perspective-correct interpolated normal in world space
    float depth; // depth in view space
    vec2 screen_pos; // pixel position on screen
    const material_props* material;
};

struct dir_light
{
    vec3 direction; // in world space
    vec3 color;
};

class shader
{
public:
    static vec3 phong(const shader_input& in, const dir_light& light, const vec3& view_pos);

    static auto make_flat_shader(const vec3& surface_color);
    static auto make_checkerboard_shader(const dir_light& light, int tiles = 4);
    static auto make_normal_shader();
    static auto make_diffuse_shader(const dir_light& light);
    static auto make_phong_shader(const dir_light& light);
    static auto make_texture_shader(const texture& tex, const dir_light& light, bool bilinear = false);
    static auto make_rainbow_shader(const dir_light& light, const float* time);
};

#include "shader.hxx"
