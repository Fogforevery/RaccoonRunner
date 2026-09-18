#pragma once

#include <array>

#include "maths/mat4.hh"
#include "maths/vec2.hh"
#include "mesh.hh"
#include "render_target.hh"
#include "scene.hh"

inline constexpr float NEAR_CLIP = -0.01f;

class rasterizer2d
{
public:
    // Signed area of the parallelogram: dot product between vectors ab and ac
    // * If the result is positive, point C is to the left of the directed line AB.
    // * If the result is negative, point C is to the right of the directed line AB.
    // * If the result is zero, point C is collinear with A and B.
    static float parallelogram_area(const vec2& a, const vec2& b, const vec2& c);

    // Project a view-space point onto the screen (see tp1 isim)
    // Returns pixel coordinates (x right, y down, origin top-left)
    static vec2 project(const vec3& view_pos, float fov, float screen_width, float screen_height);

    // Interpolate all attributes of two rast_vertices
    // Used to compute the clip point on the near plane
    static rast_vertex lerp_vertex(const rast_vertex& a, const rast_vertex& b, float t);

    // Clip a single rast_vertex against z = NEAR_CLIP
    // Returns a list of 0, 1, or 2 output triangles (each is three RastVertices)
    static unsigned clip_triangle(const rast_vertex& v0, const rast_vertex& v1, const rast_vertex& v2,
                                  std::array<rast_vertex, 6>& out);

    // Breaking news: this function rasterizes a triangle!
    template <typename shader_fn>
    static void rasterize_triangle(render_target& rt, float fov, rast_vertex& a, rast_vertex& b, rast_vertex& c,
                                   const material_props* material, shader_fn shade);

    // Draw a triangle by clipping it and rasterizing it
    template <typename shader_fn>
    static void draw_triangle(render_target& rt, float fov, rast_vertex& a, rast_vertex& b, rast_vertex& c,
                              const material_props* material, shader_fn shade);

    // Rasterize an entire mesh
    template <typename shader_fn>
    static void draw_mesh(render_target& rt, const mesh& mesh, const mat4& model, const mat4& mv, float fov,
                          shader_fn shade);

    // Render a scene with all its meshs
    static void render_scene(render_target& rt, scene& scene);
};

#include "rasterizer2d.hxx"
