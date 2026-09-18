#include <raylib.h>

#include "app/app.hh"

int main(void)
{
    const unsigned INTERNAL_WIDTH = 960; // 480 or 960
    const unsigned INTERNAL_HEIGHT = 540; // 270 or 540

    const float UPSCALE_FACTOR = 1.5f;

    const screen_dim dims = {
        .internal_width = INTERNAL_WIDTH,
        .internal_height = INTERNAL_HEIGHT,
        .target_width = static_cast<unsigned>(INTERNAL_WIDTH * UPSCALE_FACTOR),
        .target_height = static_cast<unsigned>(INTERNAL_HEIGHT * UPSCALE_FACTOR),
        .upscale_factor = UPSCALE_FACTOR,
    };

    InitWindow(dims.target_width, dims.target_height, "Raccoon Runner");
    InitAudioDevice();
    SetTargetFPS(30);

    Image image = GenImageColor(dims.internal_width, dims.internal_height, BLACK);
    Texture2D raylib_tex = LoadTextureFromImage(image);
    SetTextureFilter(raylib_tex, TEXTURE_FILTER_BILINEAR); // or TEXTURE_FILTER_POINT
    UnloadImage(image);

    app application{ dims };

    while (application.update())
        application.draw(raylib_tex);

    UnloadTexture(raylib_tex);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
