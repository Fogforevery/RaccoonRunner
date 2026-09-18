#pragma once

#include <raylib.h>

class home_screen
{
public:
    home_screen(int width, int height, float upscale, unsigned int last_score = 0, unsigned int best_score = 0);
    ~home_screen();

    // Returns true when the user has clicked the Play button.
    bool update();

    void draw() const;

private:
    static constexpr const char* HOME_MUSIC_NAME = "assets/musics/froghorn-exe.mp3";

    int width_;
    int height_;
    float upscale_;

    Rectangle play_button_;
    bool button_hovered_;

    Music music_;
    bool music_loaded_;

    unsigned last_score_;
    unsigned best_score_;
};
