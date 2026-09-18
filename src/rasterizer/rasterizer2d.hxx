#pragma once

#include <algorithm>
#include <thread>

#include "maths/maths.hh"
#include "rasterizer2d.hh"
#include "shader.hh"

template <typename shader_fn>
inline void rasterizer2d::rasterize_triangle(render_target& rt, float fov, rast_vertex& a, rast_vertex& b,
                                             rast_vertex& c, const material_props* material, shader_fn shade)
{
    int width = rt.get_width();
    int height = rt.get_height();
    float fwidth = (float)width;
    float fheight = (float)height;

    auto& depths = rt.get_depths();
    auto& pixels = rt.get_pixels();

    a.screen_pos = project(a.view_pos, fov, fwidth, fheight);
    b.screen_pos = project(b.view_pos, fov, fwidth, fheight);
    c.screen_pos = project(c.view_pos, fov, fwidth, fheight);

    float total_area = parallelogram_area(a.screen_pos, b.screen_pos, c.screen_pos);

    // Back-face culling: triangle is facing away, do not display it
    if (total_area <= 0.0f)
        return;

    // Bounding box
    int x_min = std::max(0, (int)std::floor(std::min({ a.screen_pos.x, b.screen_pos.x, c.screen_pos.x })));
    int y_min = std::max(0, (int)std::floor(std::min({ a.screen_pos.y, b.screen_pos.y, c.screen_pos.y })));
    int x_max = std::min(width - 1, (int)std::ceil(std::max({ a.screen_pos.x, b.screen_pos.x, c.screen_pos.x })));
    int y_max = std::min(height - 1, (int)std::ceil(std::max({ a.screen_pos.y, b.screen_pos.y, c.screen_pos.y })));

    float inv_za = -1.0f / a.view_pos.z;
    float inv_zb = -1.0f / b.view_pos.z;
    float inv_zc = -1.0f / c.view_pos.z;

    vec2 uv_a = a.uv * inv_za;
    vec2 uv_b = b.uv * inv_zb;
    vec2 uv_c = c.uv * inv_zc;

    vec3 world_normal_a = a.world_normal * inv_za;
    vec3 world_normal_b = b.world_normal * inv_zb;
    vec3 world_normal_c = c.world_normal * inv_zc;

    vec3 view_pos_a = a.view_pos * inv_za;
    vec3 view_pos_b = b.view_pos * inv_zb;
    vec3 view_pos_c = c.view_pos * inv_zc;

    for (int y = y_min; y <= y_max; y++)
    {
        for (int x = x_min; x <= x_max; x++)
        {
            vec2 p{ x + 0.5f, y + 0.5f }; // sample at pixel center (better for precision)

            // Barycenter weights: how much each vertex does this pixel belong to?
            float bcp_area = parallelogram_area(b.screen_pos, c.screen_pos, p);
            float cap_area = parallelogram_area(c.screen_pos, a.screen_pos, p);
            float abp_area = parallelogram_area(a.screen_pos, b.screen_pos, p);

            // Do not draw points outside of the triangle
            if (bcp_area < 0.0f || cap_area < 0.0f || abp_area < 0.0f)
                continue;

            float wa = bcp_area / total_area;
            float wb = cap_area / total_area;
            float wc = abp_area / total_area;

            // Interpolate linearly in screen space (1/z)
            float inv_z = wa * inv_za + wb * inv_zb + wc * inv_zc;
            float depth = 1.0f / inv_z;

            int idx = y * fwidth + x;
            float cur_depth = depths[idx].load(std::memory_order_relaxed);

            while (depth < cur_depth)
            {
                // Try to write the depth if and only if its value has not changed since its has been loaded
                if (depths[idx].compare_exchange_weak(cur_depth, depth, std::memory_order_relaxed,
                                                      std::memory_order_relaxed))
                {
                    // Race won, continue by writing the pixel...
                    break;
                }
            }

            // Check in another pixel has been written in the meanwhile
            if (depths[idx].load(std::memory_order_relaxed) != depth)
                continue;

            shader_input si;
            // All attributes must be perspective-corrected through 1/z before being interpolated across pixels
            si.uv = (uv_a * wa + uv_b * wb + uv_c * wc) * depth;
            si.world_normal = (world_normal_a * wa + world_normal_b * wb + world_normal_c * wc) * depth;
            si.depth = depth;
            si.screen_pos = p;
            si.material = material;

            vec3 view_pos = (view_pos_a * wa + view_pos_b * wb + view_pos_c * wc) * depth;

            // Pixels does not need to be atomic here, because it is written only after passing the depth test—only
            // one thread can win for a given pixel. The color is therefore written without contention.
            pixels[idx] = render_target::to_color(shade(si, view_pos));
        }
    }
}

template <typename shader_fn>
inline void rasterizer2d::draw_triangle(render_target& rt, float fov, rast_vertex& a, rast_vertex& b, rast_vertex& c,
                                        const material_props* material, shader_fn shade)
{
    std::array<rast_vertex, 6> clipped;
    unsigned n = clip_triangle(a, b, c, clipped);

    for (unsigned i = 0; i < n; i++)
        rasterize_triangle(rt, fov, clipped[3 * i + 0], clipped[3 * i + 1], clipped[3 * i + 2], material, shade);
}

template <typename shader_fn>
inline void rasterizer2d::draw_mesh(render_target& rt, const mesh& mesh, const mat4& model, const mat4& mv, float fov,
                                    shader_fn shade)
{
    unsigned num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    const std::vector<triangle>& triangles = mesh.get_triangles();
    size_t triangle_count = triangles.size();
    size_t chunk = (triangle_count + num_threads - 1) / num_threads;

    auto worker = [&](size_t start, size_t end) {
        for (size_t t = start; t < end; t++)
        {
            const triangle& tri = triangles[t];

            const vertex& va = tri.vertices[0];
            const vertex& vb = tri.vertices[1];
            const vertex& vc = tri.vertices[2];

            // Convert vertex to rast_vertex

            rast_vertex ra, rb, rc;

            ra.world_pos = model.transform_point(va.position);
            rb.world_pos = model.transform_point(vb.position);
            rc.world_pos = model.transform_point(vc.position);

            ra.view_pos = mv.transform_point(va.position);
            rb.view_pos = mv.transform_point(vb.position);
            rc.view_pos = mv.transform_point(vc.position);

            ra.world_normal = vec3::normalize(model.transform_dir(va.normal));
            rb.world_normal = vec3::normalize(model.transform_dir(vb.normal));
            rc.world_normal = vec3::normalize(model.transform_dir(vc.normal));

            ra.uv = va.uv;
            rb.uv = vb.uv;
            rc.uv = vc.uv;

            draw_triangle(rt, fov, ra, rb, rc, tri.material, shade);
        }
    };

    for (unsigned t = 0; t < num_threads; t++)
    {
        size_t start = t * chunk;
        size_t end = std::min(start + chunk, triangle_count);
        threads.emplace_back(worker, start, end);
    }

    for (auto& th : threads)
        th.join();
}
