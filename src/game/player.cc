#include "player.hh"

#include <cmath>
#include <raylib.h>

player::player()
    : current_lane_(DEFAULT_LANE)
    , target_lane_(DEFAULT_LANE)
    , lane_x_(LANE_X[DEFAULT_LANE])
    , jump_state_(jump_state::on_ground)
    , vel_y_(0.0f)
{
    trans_.set_position({ lane_x_, GROUND_Y, -15.0f });
    trans_.set_rotation({ 0.0f, maths::MY_PI, 0.0f });
    trans_.set_scale({ 0.5f, 0.5f, 0.5f });
}

player::player(std::shared_ptr<scene_object> player_obj)
    : player_obj_(player_obj)
    , current_lane_(DEFAULT_LANE)
    , target_lane_(DEFAULT_LANE)
    , lane_x_(LANE_X[DEFAULT_LANE])
    , jump_state_(jump_state::on_ground)
    , vel_y_(0.0f)
{
    trans_.set_position({ lane_x_, GROUND_Y, -15.0f });
    trans_.set_rotation({ 0.0f, maths::MY_PI, 0.0f });
    trans_.set_scale({ 0.5f, 0.5f, 0.5f });
    player_obj_->get_transform().set_rotation(trans_.get_rotation());
    player_obj_->get_transform().set_scale(trans_.get_scale());
}

void player::handle_input()
{
    // Move horizontaly
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
    {
        if (target_lane_ > 0)
            target_lane_--;
    }
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
    {
        if (target_lane_ < 2)
            target_lane_++;
    }

    // Move vertically
    if (IsKeyPressed(KEY_SPACE) && jump_state_ == jump_state::on_ground)
    {
        jump_state_ = jump_state::jumping;
        vel_y_ = JUMP_VELOCITY;
    }
}

void player::update_physics(float dt)
{
    // Lane sliding
    float target_x = LANE_X[target_lane_];
    // Exponential smoothing (frame-rate independent)
    lane_x_ += (target_x - lane_x_) * (1.0f - std::exp(-LANE_LERP_SPEED * dt));

    // Jump / fall
    if (jump_state_ != jump_state::on_ground)
    {
        if (jump_state_ == jump_state::jumping)
            vel_y_ += GRAVITY_JUMPING * dt;
        else
            vel_y_ += GRAVITY_FALLING * dt;

        // Switch from jumping to falling once the apex is reached
        if (jump_state_ == jump_state::jumping && vel_y_ <= 0.0f)
            jump_state_ = jump_state::falling;

        float new_y = trans_.get_position().y + vel_y_ * dt;

        if (new_y <= GROUND_Y)
        {
            new_y = GROUND_Y;
            vel_y_ = 0.0f;
            jump_state_ = jump_state::on_ground;
        }

        trans_.set_position_y(new_y);
    }

    trans_.set_position_x(lane_x_);
}

void player::update_transform()
{
    player_obj_->get_transform().set_position(trans_.get_position());
}

transform& player::get_transform()
{
    return trans_;
}

const transform& player::get_transform() const
{
    return trans_;
}
