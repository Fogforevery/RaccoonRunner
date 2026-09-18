#pragma once

#include "maths/transform.hh"
#include "maths/vec3.hh"
#include "rasterizer/scene_object.hh"

enum class jump_state
{
    on_ground,
    jumping,
    falling,
};

class player
{
public:
    player();
    player(std::shared_ptr<scene_object> player_obj);

    // Must be called before update_physics
    void handle_input();

    void update_physics(float dt);
    void update_transform();

    transform& get_transform();
    const transform& get_transform() const;

    // Exposes the underlying scene object so the game can swap shaders
    std::shared_ptr<scene_object> get_player_obj() const
    {
        return player_obj_;
    }

private:
    static constexpr float LANE_X[3] = { -9.0f, 0.0f, 9.0f };
    static constexpr int DEFAULT_LANE = 1; // start in the middle lane

    static constexpr float LANE_LERP_SPEED = 10.0f; // how fast we slide between lanes
    static constexpr float JUMP_VELOCITY = 70.0f; // initial upward velocity (units/s)
    static constexpr float GRAVITY_FALLING = -40.0f; // downward acceleration (units/s^2)
    static constexpr float GRAVITY_JUMPING = -160.0f; // downward acceleration (units/s^2)
    static constexpr float GROUND_Y = 0.0f;

    std::shared_ptr<scene_object> player_obj_;
    transform trans_;

    int current_lane_; // index into LANE_X
    int target_lane_; // lane we are sliding towards
    float lane_x_; // current interpolated X position

    jump_state jump_state_;
    float vel_y_; // vertical velocity (units/s)
};
