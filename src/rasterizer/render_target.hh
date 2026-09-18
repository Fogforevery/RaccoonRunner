#pragma once

#include <atomic>
#include <memory>
#include <raylib.h>
#include <vector>

#include "maths/vec3.hh"
#include "texture.hh"

class render_target
{
public:
    render_target(int width, int height);

    void clear(const Color& color);
    // If no texture has been loaded, the below function calls the above classic clear function with BLACK color.
    void clear_with_skybox();

    unsigned get_width() const;
    unsigned get_height() const;

    std::vector<Color>& get_pixels();
    const std::vector<Color>& get_pixels() const;

    void set_pixel(int x, int y, const Color& color);

    std::vector<std::atomic<float>>& get_depths();
    const std::vector<std::atomic<float>>& get_depths() const;

    void set_skybox(std::unique_ptr<texture> skybox);

    static Color to_color(const vec3& c);

private:
    int width_;
    int height_;
    std::vector<Color> pixels_;
    std::vector<std::atomic<float>> depths_;
    std::unique_ptr<texture> skybox_;
    std::vector<Color> skybox_cache_;
};
