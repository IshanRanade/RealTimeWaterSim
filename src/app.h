#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdexcept>
#include <optional>


struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
};

class App {
public:
    App();
    ~App();
    int start();
    void setupVulkan();
    
    GLFWwindow *window;
    
private:
    VkInstance vkInstance = VK_NULL_HANDLE;
    VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
    
    
    
    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};


