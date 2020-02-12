#include <iostream>
#include <vector>
#include <json.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <mos/gfx/renderer.hpp>
#include <mos/gfx/assets.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gfx/light.hpp>
#include <mos/gfx/environment_light.hpp>
#include <mos/gfx/scene.hpp>
#include <mos/aud/sound.hpp>
#include <mos/aud/renderer.hpp>
#include <mos/util.hpp>
#include <mos/io/window.hpp>
#include <string>
#include <filesystem/path.h>
#include <glm/gtc/color_space.hpp>
#include <glm/gtc/random.hpp>

int main() {
  glm::vec2 resolution = glm::vec2(1920, 1080) / 1.0f;
  mos::io::Window window("Skeleton", resolution);
  window.key_func = [&](int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
      window.close(true);
    }
  };

  mos::gfx::Assets gfx_assets;
  mos::aud::Assets aud_assets;
  mos::gfx::Models models;
  mos::aud::Sounds sounds;
  mos::gfx::Cloud point_cloud;
  mos::gfx::Cloud line_cloud;
  //particle_cloud.emission_map = gfx_assets.texture("particle.png");
  //line_cloud.texture = gfx_assets.texture("line.png");
  std::vector<float> velocities;
  for (auto i = 0; i < 10000; i++){
    auto p = mos::gfx::Point(glm::linearRand(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 2.0f)));
    p.size = glm::linearRand(0.0f, 0.20f);
    p.color = glm::linearRand(glm::vec4(0.0f), glm::vec4(1.0));
    p.alpha = glm::linearRand(0.0f, 0.8f);
    point_cloud.points.push_back(p);
    velocities.push_back(glm::linearRand(0.0f, 0.3f));
  }
  for (auto i = 0; i < 1000; i++) {
    auto p = mos::gfx::Point(glm::linearRand(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 2.0f)));
    p.size = glm::linearRand(0.0f, 0.20f);
    p.color = glm::linearRand(glm::vec4(0.0f), glm::vec4(1.0));
    p.alpha = glm::linearRand(0.0f, 0.8f);
    line_cloud.points.push_back(p);
    auto p1 = p;
    p1.position -= glm::vec3(0.0f, 0.0f, 0.3f);
    line_cloud.points.push_back(p1);
  }

  mos::gfx::Text text("MOS",
                      mos::gfx::Font("assets/fonts/noto_sans_regular_48.json"),
                      glm::translate(glm::mat4(1.0f), glm::vec3(-0.4, 0.6f, 1.0f))
                          * glm::rotate(glm::mat4(1.0f),
                                        glm::half_pi<float>(),
                                        glm::vec3(1.0f, 0.0f, 0.0f))
                          * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)));

  std::vector<mos::gfx::Environment_light> environment_lights;
  std::vector<mos::gfx::Light> lights;

  auto source = mos::text("assets/skeleton.level");
  auto doc = nlohmann::json::parse(source);
  for (auto &value : doc) {
	 std::string t = value;
    auto path = filesystem::path(t);
    auto type = path.extension();
      if (type == "model") {
        mos::gfx::Model model = mos::gfx::Model(path.str(), gfx_assets);
        models.push_back(model);
      }
      else if (type == "sound") {
          sounds.push_back(mos::aud::Sound(path.str(), aud_assets));
          sounds.back().source.playing = true;
          sounds.back().source.loop = true;
      }
      else if (type == "environment_light") {
        environment_lights.emplace_back(mos::gfx::Environment_light("assets/", path.str()));
      }
      else if (type == "light") {
        lights.emplace_back(mos::gfx::Light("assets/", path.str()));
      }
  }

  mos::gfx::Renderer gfx_renderer(resolution, 4);
  mos::aud::Renderer aud_renderer;


  mos::gfx::Camera camera(glm::vec3(0.0f, -3.5f, 1.72f),
                          glm::vec3(0.0f, 0.0f, 0.85f),
                          glm::perspective(0.78f, resolution.x / resolution.y, 0.1f, 100.0f));
  camera.fstop = 0.1f;

  models.push_back(text.model());

  mos::gfx::Scene scene(models,
                        camera,
                        {lights.at(0), mos::gfx::Light(), mos::gfx::Light(), mos::gfx::Light()},
                        mos::gfx::Fog(glm::vec3(0.0f), glm::vec3(0.0f), 0.0f),
                        {environment_lights.back(), mos::gfx::Environment_light()});
  scene.point_clouds = {point_cloud};
  scene.line_clouds ={line_cloud};

  std::chrono::duration<float> frame_time =
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::seconds(0));
  float time = 0.0f;

  mos::aud::Scene aud_scene(sounds, {}, camera.position());

  while (!window.close()) {
    for (int i = 0; i < scene.point_clouds[0].points.size(); i++) {
      auto & p = scene.point_clouds[0].points[i];
      p.position.z -= frame_time.count() * velocities[i];
      if(p.position.z < 0.0f){
        p.position.z = 2.0f;
      }
    }
    for (int i = 0; i < scene.line_clouds[0].points.size(); i++) {
      auto & p = scene.line_clouds[0].points[i];
      p.position.z -= frame_time.count() * velocities[i];
      if(p.position.z < 0.0f){
        p.position.z = 2.0f;
      }
    }
    const auto start_time = std::chrono::high_resolution_clock::now();

    auto center = scene.lights[0].center();
    center.x = glm::sin(time * 0.5f);
    center.y = glm::sin(time * 0.2f);
    scene.lights[0].center(center);

    gfx_renderer.render({scene}, glm::vec4(0.0f, 0.0f, 0.0, 0.0f), resolution);

    aud_scene.sounds.back().source.position = scene.lights[0].center();
    aud_renderer.render(aud_scene);

    window.poll_events();
    window.swap_buffers();
    frame_time = std::chrono::high_resolution_clock::now() - start_time;
    time += frame_time.count();
  }

  return 0;
}
