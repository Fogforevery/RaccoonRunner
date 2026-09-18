#include "game.hh"

#include <algorithm>
#include <iostream>
#include <raylib.h>

#include "maths/maths.hh"
#include "rasterizer/rasterizer2d.hh"

const std::vector<std::string> game::GAME_TRACKS = {
    "assets/musics/bad-piggies-hardstyle.mp3",
    "assets/musics/bad-piggies-phonk.mp3",
    "assets/musics/free-bird.mp3",
    "assets/musics/gas-gas-gas.mp3",
    "assets/musics/hier-encore.mp3",
    "assets/musics/subway-surfer-phonk.mp3",
};

game::game(unsigned width, unsigned height)
    : rt_(width, height)
    , yaw_(0.0f)
    , pitch_(0.0f)
    , rainbow_time_(0.0f)
    , rainbow_timer_(0.0f)
    , score_(0.0f)
{
    rt_.set_skybox(std::make_unique<texture>(texture::load_from_ppm("assets/textures/night-sky2.ppm")));

    // Load meshes
    road_mesh_ = obj_loader::load("assets/models/road.obj");
    raccoon_mesh_ = obj_loader::load("assets/models/raccoon-jules.obj");
    trashbag_mesh_ = obj_loader::load("assets/models/trashbag.obj");
    trashcan_mesh_ = obj_loader::load("assets/models/trashcan.obj");
    star_mesh_ = obj_loader::load("assets/models/star.obj");

    // Camera
    sc_.get_camera().get_transform().set_position({ 0.0f, 10.0f, -2.0f });
    sc_.get_camera().get_transform().set_rotation({ -0.4f, 0.0f, 0.0f });
    sc_.get_camera().get_transform().set_scale({ 1.0f, 1.0f, 1.0f });
    sc_.get_camera().set_fov(maths::to_radians(60.0f));

    // Light
    sc_.get_light().direction = { -1.0f, -1.0f, -1.0f };
    sc_.get_light().color = { 0.6f, 0.6f, 0.8f };

    // Raccoon (player)
    auto player_shader = shader::make_phong_shader(sc_.get_light());
    player_ = player(sc_.add(&raccoon_mesh_, nullptr, player_shader));
    dead_ = false;

    // Stars
    auto star_shader = shader::make_phong_shader(sc_.get_light());
    std::vector<std::shared_ptr<scene_object>> stars;
    stars.push_back(sc_.add(&star_mesh_, nullptr, star_shader));
    stars.push_back(sc_.add(&star_mesh_, nullptr, star_shader));
    stars.push_back(sc_.add(&star_mesh_, nullptr, star_shader));

    // Road
    auto road_shader = shader::make_diffuse_shader(sc_.get_light());
    std::vector<std::shared_ptr<scene_object>> roads;
    roads.push_back(sc_.add(&road_mesh_, nullptr, road_shader));
    roads.push_back(sc_.add(&road_mesh_, nullptr, road_shader));
    roads.push_back(sc_.add(&road_mesh_, nullptr, road_shader));
    road_objs_ = roads;

    // Obstacles
    std::vector<std::shared_ptr<scene_object>> obstacles;
    obstacles.push_back(sc_.add(&trashbag_mesh_, nullptr, road_shader));
    obstacles.push_back(sc_.add(&trashcan_mesh_, nullptr, road_shader));
    obstacles.push_back(sc_.add(&trashbag_mesh_, nullptr, road_shader));
    obstacles.push_back(sc_.add(&trashcan_mesh_, nullptr, road_shader));
    obstacles.push_back(sc_.add(&trashbag_mesh_, nullptr, road_shader));
    obstacles.push_back(sc_.add(&trashcan_mesh_, nullptr, road_shader));

    world_ = world(roads, obstacles, stars);

    // Music
    music_loaded_ = false;
    srand((unsigned)time(nullptr));
    const std::string& track = GAME_TRACKS[rand() % GAME_TRACKS.size()];
    if (FileExists(track.c_str()))
    {
        music_ = LoadMusicStream(track.c_str());
        music_loaded_ = true;
        SetMusicVolume(music_, 0.6f);
        PlayMusicStream(music_);
    }
    else
    {
        std::cerr << "WARNING: Music '" << track.c_str() << "' could not have been loaded";
    }
}

game::~game()
{
    if (music_loaded_)
    {
        StopMusicStream(music_);
        UnloadMusicStream(music_);
    }
}

const render_target& game::get_render_target()
{
    return rt_;
}

unsigned int game::get_score() const
{
    return (unsigned int)score_;
}

void game::handle_camera_input()
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (IsCursorHidden())
            EnableCursor();
        else
            DisableCursor();
    }

    if (!IsCursorHidden())
        return;

    Vector2 mouse_delta = GetMouseDelta();
    yaw_ -= mouse_delta.x * 0.002f;
    pitch_ -= mouse_delta.y * 0.002f;
    pitch_ = std::clamp(pitch_, -1.4f, 1.4f);

    sc_.get_camera().get_transform().set_rotation({ pitch_, yaw_, 0.0f });
}

void game::rebuild_player_shader()
{
    if (rainbow_timer_ > 0.0f)
    {
        player_.get_player_obj()->set_shader(shader::make_rainbow_shader(sc_.get_light(), &rainbow_time_));
        for (auto& road : road_objs_)
            road->set_shader(shader::make_rainbow_shader(sc_.get_light(), &rainbow_time_));
        world_.set_speed(world::RAINBOW_SPEED_BOOST);
    }
    else
    {
        player_.get_player_obj()->set_shader(shader::make_phong_shader(sc_.get_light()));
        auto road_shader = shader::make_diffuse_shader(sc_.get_light());
        for (auto& road : road_objs_)
            road->set_shader(road_shader);
        world_.set_speed(world::BASE_SPEED);
    }
}

bool game::update(float dt)
{
    // Music update
    if (music_loaded_)
        UpdateMusicStream(music_);

    // Player update
    player_.handle_input();
    player_.update_physics(dt);
    player_.update_transform();

    // World update
    world_.update_physics(dt);

    // Increment player score
    score_ += world_.get_speed() * dt;

    // Rainbow timer
    bool was_rainbow = rainbow_timer_ > 0.0f;

    if (rainbow_timer_ > 0.0f)
    {
        rainbow_timer_ -= dt;
        if (rainbow_timer_ < 0.0f)
            rainbow_timer_ = 0.0f;

        // Update the time value read by the shader every frame
        rainbow_time_ = (float)GetTime(); // use dt??

        // Transition back to diffuse phong when the effect expires
        if (was_rainbow && rainbow_timer_ <= 0.0f)
            rebuild_player_shader();
    }

    // Star collection
    for (std::size_t i = 0; i < world_.get_stars().size(); i++)
    {
        const auto& st = world_.get_stars()[i]->get_transform();
        const auto& pp = player_.get_transform();

        if (std::abs(pp.get_position_x() - st.get_position_x()) <= STAR_COLLECT_SIZE_X
            && std::abs(pp.get_position_y() - st.get_position_y()) <= STAR_COLLECT_SIZE_Y
            && std::abs(pp.get_position_z() - st.get_position_z()) <= STAR_COLLECT_SIZE_Z)
        {
            world_.collect_star(i);

            bool just_activated = (rainbow_timer_ <= 0.0f);
            rainbow_timer_ = RAINBOW_DURATION;
            rainbow_time_ = (float)GetTime();

            if (just_activated)
                rebuild_player_shader();
        }
    }

    // Obstacle collision
    for (std::size_t i = 0; i < world_.get_obstacles().size(); i++)
    {
        const auto& obs = world_.get_obstacles()[i]->get_transform();
        const auto& pp = player_.get_transform();

        if (std::abs(pp.get_position_x() - obs.get_position_x()) <= COLLISION_SIZE_X
            && std::abs(pp.get_position_y() - obs.get_position_y()) <= COLLISION_SIZE_Y
            && std::abs(pp.get_position_z() - obs.get_position_z()) <= COLLISION_SIZE_Z)
        {
            dead_ = true;
        }
    }

    // Camera
    handle_camera_input();

    // Render
    rt_.clear_with_skybox();
    rasterizer2d::render_scene(rt_, sc_);
    return dead_;
}
