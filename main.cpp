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
#include <mos/util.hpp>
#include <mos/io/window.hpp>

int main() {
  glm::vec2 resolution = glm::vec2(1920, 1080) / 2.4f;
  mos::io::Window window("Skeleton", resolution);
  window.key_func = [&](int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
      window.close(true);
    }
  };

  mos::gfx::Assets gfx_assets;
  std::vector<mos::gfx::Model> models;

  auto source = mos::text("assets/skeleton.level");
  auto doc = nlohmann::json::parse(source);
  for (auto &value : doc) {
      std::cout << value << std::endl;
      mos::gfx::Model model = gfx_assets.model(value);
      models.push_back(model);
  }

  mos::gfx::Renderer gfx_renderer;

  mos::gfx::Light light(glm::vec3(0.0f, 0.0f, 1.9f),
                        glm::vec3(0.0f, 0.0f, 0.0f),
                        1.3f,
                        glm::vec3(1.0f, 0.84f, 0.67f), 100.0f);

  mos::gfx::Camera camera(glm::vec3(0.0f, -3.5f, 1.0f),
                          glm::vec3(0.0f, 0.0f, 1.0),
                          glm::perspective(0.78f, resolution.x / resolution.y, 0.1f, 100.0f));

  mos::gfx::EnvironmentLight environment_light(glm::vec3(0.0f, 0.0f, 1.5f),
                                               glm::vec3(1.01f, 1.01f, 2.01f));

  mos::gfx::Scene scene(
      models.begin(),
      models.end(),
      camera,
      light,
      environment_light,
      mos::gfx::Fog(glm::vec3(0.0f),
                    glm::vec3(0.0f), 0.0f));

  std::chrono::duration<float> frame_time =
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::seconds(0));
  float time = 0.0f;

  while (!window.close()) {
    const auto start_time = std::chrono::high_resolution_clock::now();
    scene.models[5].transform = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.25f));
    scene.models[6].transform = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.25f));
    scene.models[7].transform = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.25f));
    scene.models[8].transform = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.25f));

    auto center = scene.light.center();
    center.x = glm::sin(time * 0.5f);
    scene.light.center(center);

    gfx_renderer.render({scene}, glm::vec4(0.05f), resolution);
    window.poll_events();
    window.swap_buffers();
    frame_time = std::chrono::high_resolution_clock::now() - start_time;
    time += frame_time.count();
  }

  return 0;
}