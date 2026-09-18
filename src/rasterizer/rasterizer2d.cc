#include "rasterizer2d.hh"

#include <algorithm>

#include "maths/maths.hh"

float rasterizer2d::parallelogram_area(const vec2& a, const vec2& b, const vec2& c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

vec2 rasterizer2d::project(const vec3& view_pos, float fov, float screen_width, float screen_height)
{
    float half_height = std::tan(fov * 0.5f);

    // Division by z: to create the perspective
    float nx = view_pos.x / (-view_pos.z * half_height * (screen_width / screen_height));
    float ny = view_pos.y / (-view_pos.z * half_height);

    // Fit the screen dimensions
    // Map [-1..1] -> [0..screen]
    float px = (nx + 1.0f) * 0.5f * screen_width;
    float py = (1.0f - ny) * 0.5f * screen_height;

    return { px, py };
}

rast_vertex rasterizer2d::lerp_vertex(const rast_vertex& a, const rast_vertex& b, float t)
{
    vec3 world_pos = a.world_pos + (b.world_pos - a.world_pos) * t;
    vec3 view_pos = a.view_pos + (b.view_pos - a.view_pos) * t;
    vec2 screen_pos = {}; // filled later at the rasterization step
    vec3 world_normal = a.world_normal + (b.world_normal - a.world_normal) * t;
    vec2 uv = a.uv + (b.uv - a.uv) * t;

    return rast_vertex{ world_pos, view_pos, screen_pos, world_normal, uv };
}

unsigned rasterizer2d::clip_triangle(const rast_vertex& v0, const rast_vertex& v1, const rast_vertex& v2,
                                     std::array<rast_vertex, 6>& out)
{
    bool f0 = v0.view_pos.z < NEAR_CLIP;
    bool f1 = v1.view_pos.z < NEAR_CLIP;
    bool f2 = v2.view_pos.z < NEAR_CLIP;
    unsigned in_front_count = f0 + f1 + f2;

    auto clip_edge = [&](const rast_vertex& a, const rast_vertex& b) -> rast_vertex {
        float t = (NEAR_CLIP - a.view_pos.z) / (b.view_pos.z - a.view_pos.z);
        return lerp_vertex(a, b, t);
    };

    switch (in_front_count)
    {
    case 0: {
        // All vertices are behind, discard
        return 0;
    }
    case 1: {
        // Two vertices are behind, produces one triangle
        const rast_vertex* vertices[3] = { &v0, &v1, &v2 };
        bool front[3] = { f0, f1, f2 };

        // Rotate so that vertices[0] is the vertex that is in front
        while (!front[0])
        {
            const rast_vertex* tmp_vertex = vertices[0];
            vertices[0] = vertices[1];
            vertices[1] = vertices[2];
            vertices[2] = tmp_vertex;

            bool tmp_front = front[0];
            front[0] = front[1];
            front[1] = front[2];
            front[2] = tmp_front;
        }

        rast_vertex c01 = clip_edge(*vertices[0], *vertices[1]);
        rast_vertex c02 = clip_edge(*vertices[0], *vertices[2]);

        out[0] = *vertices[0];
        out[1] = c01;
        out[2] = c02;

        return 1;
    }
    case 2: {
        // One vertex is behind, produces two triangles
        const rast_vertex* vertices[3] = { &v0, &v1, &v2 };
        bool front[3] = { f0, f1, f2 };

        // Rotate so that vertices[0] is the vertex that is behind
        while (front[0])
        {
            const rast_vertex* tmp_vertex = vertices[0];
            vertices[0] = vertices[1];
            vertices[1] = vertices[2];
            vertices[2] = tmp_vertex;

            bool tmp_front = front[0];
            front[0] = front[1];
            front[1] = front[2];
            front[2] = tmp_front;
        }

        rast_vertex c01 = clip_edge(*vertices[0], *vertices[1]);
        rast_vertex c02 = clip_edge(*vertices[0], *vertices[2]);

        out[0] = c01;
        out[1] = *vertices[1];
        out[2] = *vertices[2];

        out[3] = c01;
        out[4] = *vertices[2];
        out[5] = c02;

        return 2;
    }
    case 3: {
        // All vertices are in front, unchanged
        out[0] = v0;
        out[1] = v1;
        out[2] = v2;
        return 1;
    }
    };

    return 0;
}

void rasterizer2d::render_scene(render_target& rt, scene& scene)
{
    mat4 view_matrix = scene.get_camera().view_matrix();

    scene.get_light().direction = vec3::normalize(scene.get_light().direction);

    for (const auto& obj : scene.get_objects())
    {
        if (!obj->get_mesh() || !obj->get_shader())
            continue;

        mat4 model = obj->get_transform().model_matrix();
        mat4 mv = view_matrix.dot(model);
        draw_mesh(rt, *obj->get_mesh(), model, mv, scene.get_camera().get_fov(), obj->get_shader());
    }
}
