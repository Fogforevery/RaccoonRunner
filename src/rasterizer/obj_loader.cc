#include "obj_loader.hh"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "maths/vec2.hh"
#include "maths/vec3.hh"

std::unordered_map<std::string, material_props>
obj_loader::load_mtl(const fs::path& path, const fs::path& texture_dir,
                     std::unordered_map<std::string, std::unique_ptr<texture>>& textures)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::ostringstream oss;
        oss << "Cannot open file '" << path << "'";
        throw std::invalid_argument(oss.str());
    }

    std::unordered_map<std::string, material_props> materials;

    std::string line;
    std::string current_name;
    material_props current_prop = {};

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string key;
        iss >> key;

        // Add previous newmtl
        if (key == "newmtl")
        {
            if (!current_name.empty())
                materials[current_name] = current_prop;

            iss >> current_name;
            current_prop = {};
        }
        else if (key == "Ns")
            iss >> current_prop.ns;
        else if (key == "Ka")
            iss >> current_prop.ka.x >> current_prop.ka.y >> current_prop.ka.z;
        else if (key == "Kd")
            iss >> current_prop.kd.x >> current_prop.kd.y >> current_prop.kd.z;
        else if (key == "Ks")
            iss >> current_prop.ks.x >> current_prop.ks.y >> current_prop.ks.z;
        else if (key == "Ke")
            iss >> current_prop.ke.x >> current_prop.ke.y >> current_prop.ke.z;
        else if (key == "Ni")
            iss >> current_prop.ni;
        else if (key == "d")
            iss >> current_prop.d;
        else if (key == "illum")
            iss >> current_prop.illum; // Only one type of illim is handled (which one?)
        // Would be cleaner with a switch thanks to a table to map strings to integers

        // _: key not found, skip it (raise a warning?)
        else if (key == "map_Kd")
        {
            // Parse map_Kd options (-o offset, -s scale) and filename
            vec2 offset{ 0.0f, 0.0f };
            vec2 scale{ 1.0f, 1.0f };
            std::string tex_filename;

            std::string token;
            while (iss >> token)
            {
                if (token == "-o")
                {
                    float ou, ov, ow;
                    if (iss >> ou >> ov >> ow)
                    {
                        offset.x = ou;
                        offset.y = ov;
                    }
                }
                else if (token == "-s")
                {
                    float su, sv, sw;
                    if (iss >> su >> sv >> sw)
                    {
                        scale.x = su;
                        scale.y = sv;
                    }
                }
                else
                {
                    // Last non-option token is the filename
                    tex_filename = token;
                }
            }

            current_prop.tex_scale = scale;
            current_prop.tex_offset = offset;

            if (!tex_filename.empty())
            {
                fs::path tex_path(tex_filename);
                std::string basename = tex_path.filename().string();
                fs::path full_tex_path = texture_dir / basename;

                // Load only if not already loaded
                if (textures.find(basename) == textures.end())
                {
                    if (fs::exists(full_tex_path))
                    {
                        try
                        {
                            auto tex = std::make_unique<texture>(texture::load_from_ppm(full_tex_path));
                            textures[basename] = std::move(tex);
                        }
                        catch (const std::exception& e)
                        {
                            throw std::runtime_error("WARNING: Failed to load texture");
                        }
                    }
                    else
                    {
                        throw std::runtime_error("WARNING: Texture file not found");
                    }
                }

                auto it = textures.find(basename);
                if (it != textures.end())
                    current_prop.map_kd = it->second.get();
            }
        }
    }

    // Add last newmtl
    if (!current_name.empty())
        materials[current_name] = current_prop;

    return materials;
}

face_index obj_loader::parse_face_token(const std::string& token)
{
    face_index fi = {};
    std::istringstream iss(token);
    std::string part;
    int slot = 0;

    while (std::getline(iss, part, '/'))
    {
        if (!part.empty())
        {
            // Be careful: OBJ index is 1-based
            int idx = std::stoi(part) - 1;

            if (slot == 0)
                fi.v = idx;
            else if (slot == 1)
                fi.vt = idx;
            else if (slot == 2)
                fi.vn = idx;
        }

        slot++;
    }

    return fi;
}

mesh obj_loader::load(const fs::path& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::ostringstream oss;
        oss << "Cannot open file '" << path << "'";
        throw std::invalid_argument(oss.str());
    }

    std::vector<vec3> positions;
    std::vector<vec2> uvs;
    std::vector<vec3> normals;

    const material_props* current_mat = nullptr;

    mesh new_mesh;

    std::string line;
    while (std::getline(file, line))
    {
        // Skip empty and comment lines
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "v")
        {
            float x, y, z;
            if (!(iss >> x >> y >> z))
                throw std::runtime_error("File format is incorrect (1)");
            positions.push_back(vec3{ x, y, z });
        }
        else if (keyword == "vt")
        {
            float u, v;
            if (!(iss >> u >> v))
                throw std::runtime_error("File format is incorrect (2)");
            uvs.push_back(vec2{ u, v });
        }
        else if (keyword == "vn")
        {
            float x, y, z;
            if (!(iss >> x >> y >> z))
                throw std::runtime_error("File format is incorrect (3)");
            normals.push_back(vec3{ x, y, z });
        }
        else if (keyword == "f")
        {
            // Collect all the tokens on the face line
            std::vector<face_index> face_vertices;

            std::string token;
            while (iss >> token)
                face_vertices.push_back(parse_face_token(token));

            // Fan triangulation
            for (std::size_t i = 1; i + 1 < face_vertices.size(); i++)
            {
                face_index fi[3] = { face_vertices[0], face_vertices[i], face_vertices[i + 1] };
                triangle tri;
                tri.material = current_mat;

                for (int j = 0; j < 3; j++)
                {
                    vertex vert;

                    vert.position =
                        (fi[j].v >= 0 && fi[j].v < (int)positions.size()) ? positions[fi[j].v] : vec3{ 0, 0, 0 };
                    vert.uv = (fi[j].vt >= 0 && fi[j].vt < (int)uvs.size()) ? uvs[fi[j].vt] : vec2{ 0, 0 };
                    vert.normal =
                        (fi[j].vn >= 0 && fi[j].vn < (int)normals.size()) ? normals[fi[j].vn] : vec3{ 0, 1, 0 };

                    tri.vertices[j] = vert;
                }

                // Add new triangle to the mesh
                new_mesh.add_triangle(tri);
            }
        }
        else if (keyword == "mtllib")
        {
            std::string mtl_file;
            iss >> mtl_file;

            fs::path texture_dir = path.parent_path().parent_path() / "textures";

            std::unordered_map<std::string, std::unique_ptr<texture>> textures;
            auto materials = load_mtl(path.parent_path() / mtl_file, texture_dir, textures);
            new_mesh.set_textures(std::move(textures));
            new_mesh.set_materials(std::move(materials));
        }
        else if (keyword == "usemtl")
        {
            std::string name;
            iss >> name;

            current_mat = new_mesh.find_material(name);
        }
    }

    return new_mesh;
}
