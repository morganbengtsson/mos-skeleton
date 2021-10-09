#include <filesystem>
#include <glm/gtc/color_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>
#include <json.hpp>
#include <mos/al/renderer.hpp>
#include <mos/aud/scene.hpp>
#include <mos/aud/sounds.hpp>
#include <mos/apu/scene.hpp>
#include <mos/gfx/assets.hpp>
#include <mos/gfx/environment_light.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gl/renderer.hpp>
#include <mos/gfx/scene.hpp>
#include <mos/gfx/spot_light.hpp>
#include <mos/gfx/text.hpp>
#include <mos/gfx/scenes.hpp>
#include <mos/io/window.hpp>
#include <mos/util.hpp>
#include <string>
#include <vector>

auto main() -> int
{
    glm::vec2 resolution = glm::vec2(1920, 1080) / 1.0f;
    mos::io::Window window("Skeleton", resolution);

    mos::gfx::Assets gfx_assets;
    mos::aud::Assets aud_assets;
    mos::gfx::Models models;
    mos::aud::Sounds sounds;
    mos::gfx::Cloud point_cloud;
    mos::gfx::Cloud line_cloud;
    std::vector<float> velocities;

    static constexpr int num_points = 10000;
    static constexpr int num_lines = 1000;

    for (auto i = 0; i < num_points; i++) {
        auto p = mos::gfx::Point(
            glm::linearRand(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 2.0f)));
        p.position += 2.0f;
        p.size = glm::linearRand(0.0f, 0.20f);
        p.color = glm::linearRand(glm::vec4(0.0f), glm::vec4(1.0));
        p.alpha = glm::linearRand(0.5f, 0.8f);
        point_cloud.points.push_back(p);
        velocities.push_back(glm::linearRand(0.0f, 0.3f));
    }

    constexpr float extent = 10.0f;
    for (auto i = 0; i < num_lines; i++) {
        auto p = mos::gfx::Point(
            glm::linearRand(glm::vec3(-extent, -extent, 0.0f), glm::vec3(extent, extent, extent/2.0f)));
        p.size = glm::linearRand(0.0f, 0.20f);
        p.color = glm::linearRand(glm::vec4(0.0f), glm::vec4(1.0));
        p.alpha = glm::linearRand(0.5f, 0.8f);
        line_cloud.points.push_back(p);
        auto p1 = p;
        p1.position -= glm::vec3(0.0f, 0.0f, 0.3f);
        line_cloud.points.push_back(p1);
    }

    mos::gfx::Text text("MOS",
                        mos::gfx::Font::load("assets/fonts/noto_sans_regular_48.json"),
                        glm::translate(glm::mat4(1.0f), glm::vec3(-0.4, 0.6f, 1.0f))
                            * glm::rotate(glm::mat4(1.0f),
                                          glm::half_pi<float>(),
                                          glm::vec3(1.0f, 0.0f, 0.0f))
                            * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)));

    std::vector<mos::gfx::Environment_light> environment_lights;
    std::vector<mos::gfx::Spot_light> spot_lights;
    mos::gfx::Directional_light directional_light;
    mos::gfx::Camera camera;

    auto source = mos::text("assets/skeleton.level");
    auto doc = nlohmann::json::parse(source);
    for (auto &value : doc) {
        std::string t = value;
        auto path = std::filesystem::path(t);
        auto type = path.extension();
        if (type == ".model") {
            mos::gfx::Model model = mos::gfx::Model::load(path.generic_string(), gfx_assets);
            models.push_back(model);
        } else if (type == ".sound") {
            sounds.push_back(mos::aud::Sound::load(path.generic_string(), aud_assets));
            sounds.back().source.playing = true;
            sounds.back().source.loop = true;
        } else if (type == ".environment_light") {
            environment_lights.emplace_back(
                mos::gfx::Environment_light::load("assets/", path.generic_string()));
        } else if (type == ".spot_light") {
            spot_lights.emplace_back(mos::gfx::Spot_light::load("assets/", path.generic_string()));
        } else if (type == ".directional_light") {
            directional_light = mos::gfx::Directional_light::load("assets/", path.generic_string());
        }
        else if (type == ".camera"){
            camera = mos::gfx::Camera::load("assets/", path.generic_string());
        }
    }

    mos::gl::Renderer gfx_renderer(resolution, 4);
    mos::al::Renderer aud_renderer;

    models.push_back(text.model());

    mos::gfx::Scenes scenes{mos::gfx::Scene(gfx_renderer.load(models),
                                            camera,
                                            {spot_lights[0],
                                             mos::gfx::Spot_light(),
                                             mos::gfx::Spot_light(),
                                             mos::gfx::Spot_light()},
                                            mos::gfx::Fog(glm::vec3(0.0f), glm::vec3(0.0f), 0.0f),
                                            {environment_lights[0], environment_lights[1]})};

    scenes[0].directional_light = directional_light;
    //scene.point_clouds = {point_cloud};
    //scene.line_clouds = {line_cloud};

    std::chrono::duration<float> frame_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::seconds(0));
    float time = 0.0f;

    //mos::aud::Scene aud_scene(sounds, {}, camera.position());

    mos::apu::Scene apu_scene;
    apu_scene.sounds = aud_renderer.load(sounds);
    apu_scene.listener = camera.position();

    while (!window.close()) {
        const auto start_time = std::chrono::high_resolution_clock::now();

        auto direction = scenes[0].spot_lights[0].direction();
        direction.x = glm::sin(time * 0.1f);
        scenes[0].spot_lights[0].direction(direction);

        /*
        for (int i = 0; i < scene.point_clouds[0].points.size(); i++) {
            auto &p = scene.point_clouds[0].points[i];
            p.position.z -= frame_time.count() * velocities[i];
            if (p.position.z < 0.0f) {
                p.position.z = 2.0f;
            }
        }

        for (int i = 0; i < scene.line_clouds[0].points.size(); i++) {
            auto &p = scene.line_clouds[0].points[i];
            p.position.z -= frame_time.count() * velocities[i];
            if (p.position.z < 0.0f) {
                p.position.z = 2.0f;
            }
        }
        */

        gfx_renderer.render(scenes, mos::hex_color(0x151322), resolution);

        apu_scene.sounds.back().source.position = scenes[0].spot_lights[0].position();
        aud_renderer.render(apu_scene, frame_time.count());

        auto input = window.poll_events();
        window.swap_buffers();
        frame_time = std::chrono::high_resolution_clock::now() - start_time;
        time += frame_time.count();

        using namespace mos::io;
        if (input.keyboard.events.erase({Keyboard::Key::Escape, Keyboard::Action::Press})) {
            window.close(true);
        }
    }

    return 0;
}
