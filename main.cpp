#include <iostream>
#include <mos/io/window.hpp>

glm::vec2 resolution = glm::vec2(1920, 1080) / 2.0f;
mos::io::Window window("Skeleton", resolution);

int main() {
  window.key_func = [&](int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
      window.close(true);
    }
  };

  while (!window.close()) {
      window.poll_events();
      window.swap_buffers();
  }

  return 0;
}