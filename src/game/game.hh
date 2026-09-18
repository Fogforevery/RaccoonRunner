#pragma once

#include "game/player.hh"
#include "game/world.hh"
#include "maths/camera.hh"
#include "rasterizer/mesh.hh"
#include "rasterizer/obj_loader.hh"
#include "rasterizer/render_target.hh"
#include "rasterizer/scene.hh"
#include "rasterizer/shader.hh"

class game
{
public:
    game(unsigned width, unsigned height);
    ~game();

    const render_target& get_render_target();
    unsigned int get_score() const;

    // Update game for each frame and return true if game is over
    bool update(float dt);

private:
    static constexpr float COLLISION_SIZE_X = 4.0f;
    static constexpr float COLLISION_SIZE_Y = 4.0f;
    static constexpr float COLLISION_SIZE_Z = 3.0f;

    // Star pick-up hitbox
    static constexpr float STAR_COLLECT_SIZE_X = 5.0f;
    static constexpr float STAR_COLLECT_SIZE_Y = 5.0f;
    static constexpr float STAR_COLLECT_SIZE_Z = 5.0f;

    // Duration of the rainbow effect after collecting a star
    static constexpr float RAINBOW_DURATION = 7.0f; // seconds

    render_target rt_;
    scene sc_;

    // Meshes (owned by game) TODO we be moved somewhere else later
    mesh road_mesh_;
    mesh raccoon_mesh_;
    mesh trashbag_mesh_;
    mesh trashcan_mesh_;
    mesh star_mesh_;

    player player_;
    world world_;

    bool dead_;

    // Camera state
    float yaw_;
    float pitch_;

    // Music
    Music music_;
    bool music_loaded_;
    static const std::vector<std::string> GAME_TRACKS;

    // Rainbow power-up state
    float rainbow_time_;
    float rainbow_timer_; // effect remaining seconds

    std::vector<std::shared_ptr<scene_object>> road_objs_;

    float score_;

    void handle_camera_input();
    void rebuild_player_shader(); // switches between phong and rainbow
};
