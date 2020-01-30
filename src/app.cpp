#include "app.h"
#include <iostream>
#include <vector>


App::App() {
    
    // Setup GLFW window
    if (!glfwInit()) {
        throw std::runtime_error("Could not initialize glfw");
    }
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(1280, 720, "Real Time Water Simulation", NULL, NULL);
    
    // Setup Vulkan
    if (!glfwVulkanSupported())
    {
        throw std::runtime_error("GLFW: Vulkan Not Supported");
    }
    
    // Setup all the vulkan members
    setupVulkan();
}

App::~App() {
    
    vkDestroyInstance(this->vkInstance, nullptr);
    
    glfwDestroyWindow(window);
    glfwTerminate();
}

void App::setupVulkan() {
    
    // Create the vulkan instance
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Real Time Water Simulation";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Emerald Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    
    createInfo.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &this->vkInstance);
    
    if (vkCreateInstance(&createInfo, nullptr, &this->vkInstance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    
    
    // Create the physical device
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(this->vkInstance, &deviceCount, nullptr);
    
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(this->vkInstance, &deviceCount, devices.data());
    
    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            this->vkPhysicalDevice = device;
            break;
        }
    }

    if (this->vkPhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
    
    
    //

}

QueueFamilyIndices App::findQueueFamilies(VkPhysicalDevice device) {
    
}

bool App::isDeviceSuitable(VkPhysicalDevice device) {
    return true;
}

int App::start() {
    
    // Display the GUI and main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
