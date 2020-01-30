#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>

class App {
public:
    App();
    ~App();
    int start();
    void setupVulkan();
    
    GLFWwindow *window;
    
private:
    VkInstance vkInstance;
};
