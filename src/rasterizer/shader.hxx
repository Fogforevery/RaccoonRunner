#pragma once

#include "shader.hh"

inline auto shader::make_flat_shader(const vec3& surface_color)
{
    return [surface_color](const shader_input&, const vec3&) -> vec3 { return surface_color; };
}

inline auto shader::make_checkerboard_shader(const dir_light& light, int tiles)
{
    return [light, tiles](const shader_input& in, const vec3&) -> vec3 {
        int cx = (int)(in.uv.x * tiles);
        int cy = (int)(in.uv.y * tiles);
        bool white = (cx + cy) % 2 == 0;
        vec3 surface_color = white ? vec3{ 0.9f, 0.9f, 0.9f } : vec3{ 0.15f, 0.15f, 0.15f };
        return surface_color;
    };
}

inline auto shader::make_normal_shader()
{
    return [](const shader_input& in, const vec3&) -> vec3 {
        vec3 n = vec3::normalize(in.world_normal);
        // Remap from [-1, 1] to [0, 1]
        return (n + vec3{ 1.0f, 1.0f, 1.0f }) * 0.5f;
    };
}

inline auto shader::make_diffuse_shader(const dir_light& light)
{
    return [light](const shader_input& in, const vec3&) -> vec3 {
        vec3 n = vec3::normalize(in.world_normal);
        vec3 l = vec3::normalize(-light.direction);

        float n_dot_l = std::max(0.0f, vec3::dot(n, l));
        const material_props& m = *in.material;

        // Use texture color as kd when available
        vec3 kd = m.kd;
        if (m.map_kd)
        {
            float u = in.uv.x * m.tex_scale.x + m.tex_offset.x;
            float v = in.uv.y * m.tex_scale.y + m.tex_offset.y;
            kd = m.map_kd->sample_bilinear(u, v);
        }

        // (1) Ambient
        vec3 ambient = light.color * m.ka * BLENDER_AMBIENT_MAGIC_VALUE;

        // (2) Diffuse
        vec3 diffuse = light.color * n_dot_l * kd;

        vec3 result = ambient + diffuse;

        return vec3::clamp(result, 0.0f, 1.0f);
    };
}

inline auto shader::make_phong_shader(const dir_light& light)
{
    return [light](const shader_input& in, const vec3& view_pos) -> vec3 { return phong(in, light, view_pos); };
}

inline auto shader::make_texture_shader(const texture& tex, const dir_light& light, bool bilinear)
{
    const texture* ptex = &tex;
    return [ptex, light, bilinear](const shader_input& in, const vec3&) -> vec3 {
        vec3 surface = bilinear ? ptex->sample_bilinear(in.uv.x, in.uv.y) : ptex->sample_nearest(in.uv.x, in.uv.y);

        // return phong(in, light, surface, view_pos);
        return surface;
    };
}

// Converts a hue in [0, 1] to an RGB color following the rainbow spectrum
inline vec3 hue_to_rgb(float h)
{
    h = h - std::floor(h); // wrap to [0, 1]
    float r = std::abs(h * 6.0f - 3.0f) - 1.0f;
    float g = 2.0f - std::abs(h * 6.0f - 2.0f);
    float b = 2.0f - std::abs(h * 6.0f - 4.0f);
    return vec3::clamp({ r, g, b }, 0.0f, 1.0f);
}

inline auto shader::make_rainbow_shader(const dir_light& light, const float* time)
{
    return [light, time](const shader_input& in, const vec3&) -> vec3 {
        vec3 n = vec3::normalize(in.world_normal);
        float hue = (n.x * 0.2 + n.y * 0.6 + n.z * 0.2) + (*time) * 0.9f;

        vec3 rainbow = hue_to_rgb(hue);

        // Diffuse shading
        vec3 l = vec3::normalize(-light.direction);
        // Keep a minimum with max so dark faces still pop
        float n_dot_l = std::max(0.2f, vec3::dot(n, l));

        vec3 result = rainbow * n_dot_l;
        return vec3::clamp(result, 0.0f, 1.0f);
    };
}
