#pragma once

#include <memory>
#include <raylib.h>

#include "app/home_screen.hh"
#include "game/game.hh"

enum class app_state
{
    home,
    playing,
};

struct screen_dim
{
    unsigned internal_width;
    unsigned internal_height;
    unsigned target_width;
    unsigned target_height;
    float upscale_factor;
};

class app
{
public:
    app(const screen_dim& dims);

    // Call every frame. Returns true while the app should keep running.
    bool update();

    // Draw the current frame.
    // For the home screen this draws via raylib directly.
    // For the game it uploads the rasterizer output and displays it.
    void draw(Texture2D& raylib_tex) const;

private:
    const screen_dim dims_;
    app_state state_;

    std::unique_ptr<home_screen> home_;
    std::unique_ptr<game> game_;
    Sound boom_;

    unsigned int last_score_;
    unsigned int best_score_;
};
