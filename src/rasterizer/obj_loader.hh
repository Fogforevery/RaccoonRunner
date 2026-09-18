#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "maths/vec3.hh"
#include "mesh.hh"
#include "texture.hh"

namespace fs = std::filesystem;

// Parse a single v/vt/vn face token
struct face_index
{
    int v;
    int vt;
    int vn;
};

class obj_loader
{
public:
    static std::unordered_map<std::string, material_props>
    load_mtl(const fs::path& path, const fs::path& texture_dir,
             std::unordered_map<std::string, std::unique_ptr<texture>>& textures);
    static face_index parse_face_token(const std::string& token);
    static mesh load(const fs::path& path);
};
