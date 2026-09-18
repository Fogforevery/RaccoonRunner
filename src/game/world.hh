#pragma once

#include <cstdlib>
#include <vector>

#include "rasterizer/scene.hh"

class world
{
public:
    static constexpr float BASE_SPEED = 250.0f;
    static constexpr float RAINBOW_SPEED_BOOST = 800.0f; // speed when a star is collected

    world();
    world(const std::vector<std::shared_ptr<scene_object>>& roads,
          const std::vector<std::shared_ptr<scene_object>>& obstacles,
          const std::vector<std::shared_ptr<scene_object>>& stars);

    float get_speed() const;
    void set_speed(float speed);

    std::vector<std::shared_ptr<scene_object>>& get_roads();
    const std::vector<std::shared_ptr<scene_object>>& get_roads() const;

    std::vector<std::shared_ptr<scene_object>>& get_obstacles();
    const std::vector<std::shared_ptr<scene_object>>& get_obstacles() const;

    std::vector<std::shared_ptr<scene_object>>& get_stars();
    const std::vector<std::shared_ptr<scene_object>>& get_stars() const;

    // Call when a star is collected: hides it and schedules a respawn far ahead
    void collect_star(std::size_t index);

    void update_physics(float dt);

private:
    static constexpr float LANE_X[3] = { -9.0f, 0.0f, 9.0f };
    static constexpr float ROAD_LENGTH = 680.0f; // length of a road piece
    static constexpr float STAR_SPACING = 5000.0f; // increase for rarity
    static constexpr float STAR_Y = 4.0f;

    float world_speed_;
    std::vector<std::shared_ptr<scene_object>> roads_;
    std::vector<std::shared_ptr<scene_object>> obstacles_;

    std::vector<std::shared_ptr<scene_object>> stars_;
    std::vector<bool> star_active_;
    float star_time_;
};
