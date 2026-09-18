#include "mesh.hh"

#include "texture.hh"

const std::vector<triangle>& mesh::get_triangles() const
{
    return triangles_;
}

void mesh::add_triangle(const triangle& tri)
{
    triangles_.push_back(tri);
}

void mesh::set_materials(std::unordered_map<std::string, material_props> materials)
{
    materials_ = std::move(materials);
}

const std::unordered_map<std::string, material_props>& mesh::get_materials() const
{
    return materials_;
}

const material_props* mesh::find_material(const std::string& name) const
{
    auto it = materials_.find(name);
    if (it == materials_.end())
        return nullptr;

    return &it->second;
}

void mesh::set_textures(std::unordered_map<std::string, std::unique_ptr<texture>> textures)
{
    textures_ = std::move(textures);
}

mesh mesh::make_cube()
{
    // Default material
    static material_props default_mat{ .ns = 32.0f,
                                       .ka = { 0.1f, 0.1f, 0.1f },
                                       .kd = { 0.8f, 0.8f, 0.8f },
                                       .ks = { 0.5f, 0.5f, 0.5f },
                                       .ke = { 0, 0, 0 },
                                       .ni = 1.0f,
                                       .d = 1.0f,
                                       .illum = 2 };

    // Helper to build one face given 4 corners, a normal, and UV corners
    // Split into two CCW triangles:
    //   tri0: v0, v1, v2
    //   tri1: v0, v2, v3
    auto add_face = [](mesh& m, const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, const vec3& normal) {
        // UVs for the four corners of each face
        vec2 uv0 = { 0, 0 };
        vec2 uv1 = { 1, 0 };
        vec2 uv2 = { 1, 1 };
        vec2 uv3 = { 0, 1 };

        // First triangle
        triangle t0;
        t0.material = &default_mat;
        t0.vertices[0] = { v0, normal, uv0 };
        t0.vertices[1] = { v1, normal, uv1 };
        t0.vertices[2] = { v2, normal, uv2 };
        m.triangles_.push_back(t0);

        // Second triangle
        triangle t1;
        t1.material = &default_mat;
        t1.vertices[0] = { v0, normal, uv0 };
        t1.vertices[1] = { v2, normal, uv2 };
        t1.vertices[2] = { v3, normal, uv3 };
        m.triangles_.push_back(t1);
    };

    mesh m;
    // back (+Z)
    add_face(m, { 1, -1, -1 }, { -1, -1, -1 }, { -1, 1, -1 }, { 1, 1, -1 }, { 0, 0, -1 });
    // front (-Z)
    add_face(m, { -1, -1, 1 }, { 1, -1, 1 }, { 1, 1, 1 }, { -1, 1, 1 }, { 0, 0, 1 });
    // left (-X)
    add_face(m, { -1, -1, -1 }, { -1, -1, 1 }, { -1, 1, 1 }, { -1, 1, -1 }, { -1, 0, 0 });
    // right (+X)
    add_face(m, { 1, -1, 1 }, { 1, -1, -1 }, { 1, 1, -1 }, { 1, 1, 1 }, { 1, 0, 0 });
    // top (+Y)
    add_face(m, { -1, 1, 1 }, { 1, 1, 1 }, { 1, 1, -1 }, { -1, 1, -1 }, { 0, 1, 0 });
    // bottom (-Y)
    add_face(m, { -1, -1, -1 }, { 1, -1, -1 }, { 1, -1, 1 }, { -1, -1, 1 }, { 0, -1, 0 });

    return m;
}
