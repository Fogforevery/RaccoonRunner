#pragma once

#include <filesystem>
#include <vector>

#include "maths/vec3.hh"

namespace fs = std::filesystem;

inline constexpr unsigned MAX_PIXEL_VALUE = 255;

class texture
{
public:
    texture() = default;
    static texture load_from_ppm(const fs::path& path);

    unsigned get_width() const;
    unsigned get_height() const;

    vec3 sample_nearest(float u, float v) const;
    vec3 sample_bilinear(float u, float v) const;

private:
    unsigned width_;
    unsigned height_;
    std::vector<vec3> pixels_;
};
