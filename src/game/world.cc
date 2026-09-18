#include "world.hh"

world::world()
    : world_speed_(BASE_SPEED)
{}

world::world(const std::vector<std::shared_ptr<scene_object>>& roads,
             const std::vector<std::shared_ptr<scene_object>>& obstacles,
             const std::vector<std::shared_ptr<scene_object>>& stars)
    : world_speed_(BASE_SPEED)
    , roads_(roads)
    , obstacles_(obstacles)
    , stars_(stars)
    , star_time_(0.0f)
{
    std::srand(std::time({}));

    for (std::size_t i = 0; i < roads_.size(); i++)
    {
        roads_[i]->get_transform().set_position({ 0.0f, -2.0f, -ROAD_LENGTH * i });
        roads_[i]->get_transform().set_rotation({ 0.0f, maths::MY_PI, 0.0f });
        roads_[i]->get_transform().set_scale({ 0.5f, 0.5f, 0.5f });
    }

    for (std::size_t i = 0; i < obstacles_.size(); i++)
    {
        obstacles_[i]->get_transform().set_position({ LANE_X[std::rand() % 3], -3.0f, (-ROAD_LENGTH / 2) * (i + 1) });
        obstacles_[i]->get_transform().set_rotation({ 0.0f, maths::MY_PI, 0.0f });
        obstacles_[i]->get_transform().set_scale({ 4.0f, 4.0f, 4.0f });
    }

    // Place stars ahead of the player, each in a random lane, well spread out
    star_active_.resize(stars_.size(), true);
    for (std::size_t i = 0; i < stars_.size(); i++)
    {
        float z = -STAR_SPACING * (i + 1);
        stars_[i]->get_transform().set_position({ LANE_X[std::rand() % 3], STAR_Y, z });
        stars_[i]->get_transform().set_rotation({ 0.0f, maths::MY_PI, 0.0f });
        stars_[i]->get_transform().set_scale({ 3.0f, 3.0f, 3.0f });
    }
}

float world::get_speed() const
{
    return world_speed_;
}

void world::set_speed(float speed)
{
    world_speed_ = speed;
}

std::vector<std::shared_ptr<scene_object>>& world::get_roads()
{
    return roads_;
}

const std::vector<std::shared_ptr<scene_object>>& world::get_roads() const
{
    return roads_;
}

std::vector<std::shared_ptr<scene_object>>& world::get_obstacles()
{
    return obstacles_;
}

const std::vector<std::shared_ptr<scene_object>>& world::get_obstacles() const
{
    return obstacles_;
}

std::vector<std::shared_ptr<scene_object>>& world::get_stars()
{
    return stars_;
}

const std::vector<std::shared_ptr<scene_object>>& world::get_stars() const
{
    return stars_;
}

void world::collect_star(std::size_t index)
{
    if (index >= stars_.size())
        return;

    star_active_[index] = false;

    // Move it very far behind so it is invisible during the cooldown
    // Update function detects that it passed the camera and respawns it
    stars_[index]->get_transform().set_position_z(-STAR_SPACING * (float)stars_.size() - 2000.0f);
}

void world::update_physics(float dt)
{
    // Roads

    for (std::size_t i = 0; i < roads_.size(); i++)
    {
        roads_[i]->get_transform().set_position_z(roads_[i]->get_transform().get_position_z() + dt * world_speed_);
        if (roads_[i]->get_transform().get_position_z() >= ROAD_LENGTH)
            roads_[i]->get_transform().set_position_z(roads_[i]->get_transform().get_position_z()
                                                      - ROAD_LENGTH * roads_.size());
    }

    // Obstacles

    for (std::size_t i = 0; i < obstacles_.size(); i++)
    {
        obstacles_[i]->get_transform().set_position_z(obstacles_[i]->get_transform().get_position_z()
                                                      + dt * world_speed_);
        if (obstacles_[i]->get_transform().get_position_z() >= ROAD_LENGTH)
        {
            obstacles_[i]->get_transform().set_position_z(obstacles_[i]->get_transform().get_position_z()
                                                          - (ROAD_LENGTH / 2) * (obstacles_.size() + 1));
            obstacles_[i]->get_transform().set_position_x(LANE_X[std::rand() % 3]);
        }
    }

    // Stars

    star_time_ += dt;

    for (std::size_t i = 0; i < stars_.size(); i++)
    {
        stars_[i]->get_transform().set_position_z(stars_[i]->get_transform().get_position_z() + dt * world_speed_);

        float z = stars_[i]->get_transform().get_position_z();

        if (z >= ROAD_LENGTH)
        {
            // Respawn far ahead in a random lane
            stars_[i]->get_transform().set_position_z(z - STAR_SPACING * stars_.size());
            stars_[i]->get_transform().set_position_x(LANE_X[std::rand() % 3]);
            stars_[i]->get_transform().set_position_y(STAR_Y + std::sin(star_time_ * 2.0f + i * 1.2f) * 1.5f);
            star_active_[i] = true;
        }
    }
}
