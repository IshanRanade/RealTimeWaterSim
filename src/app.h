#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdexcept>
#include <optional>


struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    
    bool isViable() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
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
    VkDevice vkDevice = VK_NULL_HANDLE;
    VkQueue vkGraphicsQueue = VK_NULL_HANDLE;
    VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
    VkQueue vkPresentQueue = VK_NULL_HANDLE;
    
    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};


