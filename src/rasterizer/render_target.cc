#include "render_target.hh"

#include <algorithm>
#include <limits>
#include <stdexcept>

render_target::render_target(int width, int height)
    : width_(width)
    , height_(height)
    , pixels_(width * height, BLACK)
    , depths_(width * height)
{
    if (width <= 0 || height <= 0)
        throw std::invalid_argument("render_target: dimensions must be stricty positive");

    for (auto& d : depths_)
        d.store(std::numeric_limits<float>::max(), std::memory_order_relaxed);
}

void render_target::clear(const Color& color)
{
    for (auto& p : pixels_)
        p = color;
    for (auto& d : depths_)
        d.store(std::numeric_limits<float>::max(), std::memory_order_relaxed);
}

void render_target::clear_with_skybox()
{
    if (skybox_ && !skybox_cache_.empty())
        std::copy(skybox_cache_.begin(), skybox_cache_.end(), pixels_.begin());
    else
        clear(BLACK); // fallback

    for (auto& d : depths_)
        d.store(std::numeric_limits<float>::max(), std::memory_order_relaxed);
}

unsigned render_target::get_width() const
{
    return width_;
}
unsigned render_target::get_height() const
{
    return height_;
}

void render_target::set_pixel(int x, int y, const Color& color)
{
    if (x < 0 || x >= width_ || y < 0 || y >= height_)
        return;

    pixels_[y * width_ + x] = color;
}

std::vector<Color>& render_target::get_pixels()
{
    return pixels_;
}

const std::vector<Color>& render_target::get_pixels() const
{
    return pixels_;
}

std::vector<std::atomic<float>>& render_target::get_depths()
{
    return depths_;
}

const std::vector<std::atomic<float>>& render_target::get_depths() const
{
    return depths_;
}

void render_target::set_skybox(std::unique_ptr<texture> skybox)
{
    skybox_ = std::move(skybox);

    skybox_cache_.resize(width_ * height_);

    for (int y = 0; y < height_; y++)
    {
        for (int x = 0; x < width_; x++)
        {
            float u = (float)x / (float)width_;
            float v = (float)y / (float)height_;

            vec3 color = skybox_->sample_bilinear(u, v);
            skybox_cache_[y * width_ + x] = to_color(color);
        }
    }
}

Color render_target::to_color(const vec3& c)
{
    auto ch = [](float v) -> unsigned char { return (unsigned char)(std::clamp(v, 0.0f, 1.0f) * 255.0f); };
    return Color{ ch(c.x), ch(c.y), ch(c.z), 255 };
}
