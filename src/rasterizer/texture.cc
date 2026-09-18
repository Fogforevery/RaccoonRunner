#include "texture.hh"

#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

texture texture::load_from_ppm(const fs::path& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::ostringstream oss;
        oss << "Cannot open file '" << path << "'";
        throw std::invalid_argument(oss.str());
    }

    std::string format;
    unsigned w, h;
    unsigned max_value;

    if (!(file >> format >> w >> h >> max_value))
        throw std::invalid_argument("Invalid field in PPM header");

    assert(format.compare("P3") == 0);
    assert(max_value == MAX_PIXEL_VALUE);

    texture tex;
    tex.width_ = w;
    tex.height_ = h;
    tex.pixels_.resize(w * h);

    for (unsigned y = 0; y < h; y++)
    {
        for (unsigned x = 0; x < w; x++)
        {
            unsigned r, g, b;
            if (!(file >> r >> g >> b))
                throw std::invalid_argument("Invalid pixel value");

            float rf = static_cast<float>(r) / MAX_PIXEL_VALUE;
            float gf = static_cast<float>(g) / MAX_PIXEL_VALUE;
            float bf = static_cast<float>(b) / MAX_PIXEL_VALUE;

            tex.pixels_[y * w + x] = vec3{ rf, gf, bf };
        }
    }

    return tex;
}

unsigned texture::get_width() const
{
    return width_;
}

unsigned texture::get_height() const
{
    return height_;
}

vec3 texture::sample_nearest(float u, float v) const
{
    // Keep fractional part for tiling
    u -= std::floor(u);
    v -= std::floor(v);

    int x = (int)(u * width_) % width_;
    int y = (int)(v * height_) % height_;

    return pixels_[y * width_ + x];
}

vec3 texture::sample_bilinear(float u, float v) const
{
    // Keep fractional part for tiling
    u -= std::floor(u);
    v -= std::floor(v);

    float fx = u * (float)width_ - 0.5f;
    float fy = v * (float)height_ - 0.5f;

    // Surronding pixels_ indices
    int x0 = (int)std::floor(fx);
    int y0 = (int)std::floor(fy);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Distance of the given pixel to x0, y0
    float tx = fx - (float)x0;
    float ty = fy - (float)y0;

    // Wrap indices (with tiling)
    x0 = ((x0 % width_) + width_) % width_;
    x1 = ((x1 % width_) + width_) % width_;
    y0 = ((y0 % height_) + height_) % height_;
    y1 = ((y1 % height_) + height_) % height_;

    // Fetch the four surrounding pixels_
    vec3 c00 = pixels_[y0 * width_ + x0]; // top-left
    vec3 c10 = pixels_[y0 * width_ + x1]; // top-right
    vec3 c01 = pixels_[y1 * width_ + x0]; // bottom-left
    vec3 c11 = pixels_[y1 * width_ + x1]; // bottom-right

    // Bilinear blend
    vec3 top = c00 + (c10 - c00) * tx; // X-axis
    vec3 bottom = c01 + (c11 - c01) * tx; // Y-axis

    return top + (bottom - top) * ty;
}
