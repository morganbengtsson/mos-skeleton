#include <filesystem>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <mos/gfx/vulkan/renderer.hpp>

auto main() -> int
{
    using namespace mos;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    auto window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);

    uint32_t extension_count;
    auto extensions_array = glfwGetRequiredInstanceExtensions(&extension_count);
    std::vector<const char *> extensions(extensions_array, extensions_array + extension_count);

    auto surface_creator = [&](VkInstance instance) {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
        return surface;
    };

    gfx::vulkan::Renderer renderer(extensions, surface_creator);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
