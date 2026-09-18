#include "shader.hh"

vec3 shader::phong(const shader_input& in, const dir_light& light, const vec3& view_pos)
{
    vec3 n = vec3::normalize(in.world_normal);
    vec3 l = vec3::normalize(-light.direction);
    vec3 v = vec3::normalize(vec3{ 0, 0, 0 } - view_pos); // view_pos is already in view space

    // n_dot_l should not be thanks to clipping (no?), but just to be sure...
    float n_dot_l = std::max(0.0f, vec3::dot(n, l));
    const material_props& m = *in.material;

    // (1) Ambient
    vec3 ambient = light.color * m.ka * BLENDER_AMBIENT_MAGIC_VALUE;

    // (2) Diffuse
    vec3 diffuse = light.color * n_dot_l * m.kd;

    // (3) Specular
    vec3 specular = { 0, 0, 0 };

    // Compute only when the surface faces the light
    if (n_dot_l > 0.0f)
    {
        vec3 r = vec3::normalize(n * n_dot_l * 2.0f - l);
        float spec = std::pow(std::max(0.0f, vec3::dot(r, v)), m.ns);
        specular = light.color * m.ks * spec;
    }

    // (4) Emissive (and also sum all other colors)
    vec3 result = ambient + diffuse + specular + m.ke;

    return vec3::clamp(result, 0.0f, 1.0f);
}
