#include "home_screen.hh"

#include <cmath>
#include <iostream>
#include <string>

home_screen::home_screen(int width, int height, float upscale, unsigned last_score, unsigned best_score)
    : width_(width)
    , height_(height)
    , upscale_(upscale)
    , button_hovered_(false)
    , music_loaded_(false)
    , last_score_(last_score)
    , best_score_(best_score)
{
    // Center the Play button
    const float btn_w = 220.f * upscale_;
    const float btn_h = 60.f * upscale_;
    play_button_ = { (width - btn_w) * 0.5f,
                     (height - btn_h) * 0.5f + 120.f * upscale_, // slightly below center
                     btn_w, btn_h };

    if (FileExists(HOME_MUSIC_NAME))
    {
        music_ = LoadMusicStream(HOME_MUSIC_NAME);
        music_loaded_ = true;
        SetMusicVolume(music_, 0.6f);
        PlayMusicStream(music_);
    }
    else
    {
        std::cerr << "WARNING: Music '" << HOME_MUSIC_NAME << "' could not have been loaded";
    }
}

home_screen::~home_screen()
{
    if (music_loaded_)
    {
        StopMusicStream(music_);
        UnloadMusicStream(music_);
    }
}

bool home_screen::update()
{
    if (music_loaded_)
        UpdateMusicStream(music_);

    Vector2 mouse = GetMousePosition();
    button_hovered_ = CheckCollisionPointRec(mouse, play_button_);

    if ((button_hovered_ && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_ENTER))
    {
        // Stop music before transitioning (in addition to the destructor)
        if (music_loaded_)
            StopMusicStream(music_);
        return true; // launch game
    }

    return false;
}

void home_screen::draw() const
{
    // Background
    ClearBackground({ 15, 15, 25, 255 });

    // Subtle animated scanline feel: horizontal gradient bands
    float t = (float)GetTime();
    for (int y = 0; y < height_; y += 16)
    {
        // float alpha = 0.04f + 0.02f * std::sin(t * 0.4f + y * 0.01f);
        float alpha = 0.04f + 0.02f * std::sin(t * 8.0f + y * 0.05f - t * 3.0f);
        DrawRectangle(0, y * upscale_, width_ * upscale_, 8, { 255, 255, 255, (unsigned char)(alpha * 255) });
    }

    // Title
    const char* title = "RACCOON RUNNER";
    int title_size = 72 * upscale_;
    int title_w = MeasureText(title, title_size);
    float title_x = (width_ - title_w) * 0.5f;
    float title_y = height_ * 0.25f;

    // Shadow
    DrawText(title, (int)title_x + 4, (int)title_y + 4, title_size, { 0, 0, 0, 180 });
    // Main
    DrawText(title, (int)title_x, (int)title_y, title_size, { 240, 210, 90, 255 });

    // Subtitle
    const char* sub = "dodge, jump, survive";
    int sub_size = 22 * upscale_;
    int sub_w = MeasureText(sub, sub_size);
    DrawText(sub, (width_ - sub_w) / 2, (int)(title_y + title_size + 14), sub_size, { 180, 180, 200, 200 });

    // Scores
    int sc_size = 20 * upscale_;
    std::string best_str = "BEST: " + (best_score_ ? std::to_string(best_score_) : "-");
    std::string last_str = "LAST: " + (last_score_ ? std::to_string(last_score_) : "-");
    int best_w = MeasureText(best_str.c_str(), sc_size);
    int last_w = MeasureText(last_str.c_str(), sc_size);
    DrawText(best_str.c_str(), (width_ - best_w) / 2, (int)(title_y + title_size + 70 * upscale_), sc_size,
             { 240, 210, 90, 255 });
    DrawText(last_str.c_str(), (width_ - last_w) / 2, (int)(title_y + title_size + 95 * upscale_), sc_size,
             { 180, 180, 200, 200 });

    // Play button
    Color btn_fill = button_hovered_ ? Color{ 240, 210, 90, 255 } : Color{ 50, 50, 80, 255 };
    Color btn_border = button_hovered_ ? Color{ 255, 240, 120, 255 } : Color{ 120, 120, 160, 255 };
    Color btn_text = button_hovered_ ? Color{ 20, 20, 30, 255 } : Color{ 220, 220, 240, 255 };

    DrawRectangleRounded(play_button_, 0.3f, 8, btn_fill);
    DrawRectangleRoundedLines(play_button_, 0.3f, 8, btn_border);

    const char* btn_label = "PLAY";
    int lbl_size = 32 * upscale_;
    int lbl_w = MeasureText(btn_label, lbl_size);
    DrawText(btn_label, (int)(play_button_.x + (play_button_.width - lbl_w) * 0.5f),
             (int)(play_button_.y + (play_button_.height - lbl_size) * 0.5f), lbl_size, btn_text);

    // Footer hint
    const char* hint = "A / D  move lane     SPACE  jump     ESC  exit";
    int hint_size = 16 * upscale_;
    int hint_w = MeasureText(hint, hint_size);
    DrawText(hint, (width_ - hint_w) / 2, height_ - 36 * upscale_, hint_size, { 120, 120, 140, 200 });
}
