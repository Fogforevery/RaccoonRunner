#include "app.hh"

#include <iostream> // TODO tempo

app::app(const screen_dim& dims)
    : dims_(dims)
    , state_(app_state::home)
    , home_(std::make_unique<home_screen>((int)dims.target_width, (int)dims.target_height, dims.upscale_factor))
    , game_(nullptr)
    , boom_(LoadSound("assets/sounds/boom.mp3"))
    , last_score_(0)
    , best_score_(0)
{}

bool app::update()
{
    if (WindowShouldClose())
        return false;

    switch (state_)
    {
    case app_state::home: {
        bool start = home_->update();
        if (start)
        {
            home_.reset(); // destroys home screen + stops music
            game_ = std::make_unique<game>(dims_.internal_width, dims_.internal_height);
            state_ = app_state::playing;
        }
        break;
    }

    case app_state::playing: {
        float dt = GetFrameTime();
        if (game_->update(dt))
        {
            PlaySound(boom_);
            last_score_ = game_->get_score();
            if (last_score_ > best_score_)
                best_score_ = last_score_;
            game_.reset();
            home_ = std::make_unique<home_screen>((int)dims_.target_width, (int)dims_.target_height,
                                                  dims_.upscale_factor, last_score_, best_score_);
            state_ = app_state::home;
        }
        break;
    }
    }

    return true;
}

void app::draw(Texture2D& raylib_tex) const
{
    BeginDrawing();

    switch (state_)
    {
    case app_state::home:
        home_->draw();
        break;

    case app_state::playing:
        UpdateTexture(raylib_tex, game_->get_render_target().get_pixels().data());
        // Upscale the texture
        Rectangle src = { 0, 0, (float)dims_.internal_width, (float)dims_.internal_height };
        Rectangle dst = { 0, 0, (float)dims_.target_width, (float)dims_.target_height };
        DrawTexturePro(raylib_tex, src, dst, { 0, 0 }, 0.0f, WHITE);
        DrawFPS(10, 10);
        std::string score_str = "SCORE: " + std::to_string(game_->get_score());
        DrawText(score_str.c_str(), dims_.target_width - 220, 10, 28, WHITE);
        break;
    }

    EndDrawing();
}
