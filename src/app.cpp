#include "app.h"
#include <iostream>
#include <vector>
#include <set>


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
    
    vkDestroyDevice(this->vkDevice, nullptr);
    vkDestroySurfaceKHR(this->vkInstance, this->vkSurface, nullptr);
    vkDestroyInstance(this->vkInstance, nullptr);
    
    glfwDestroyWindow(window);
    glfwTerminate();
}

void App::setupVulkan() {
    
    std::cout << "Setting up Vulkan objects..." << std::endl;
    
    // Create the vulkan instance
    {
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
        
        if (vkCreateInstance(&createInfo, nullptr, &this->vkInstance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }
    
    
    // Now create the window surface
       {
           if (glfwCreateWindowSurface(this->vkInstance, window, nullptr, &this->vkSurface) != VK_SUCCESS) {
               throw std::runtime_error("failed to create window surface!");
           }
       }
    
    
    // Create the physical device
    {
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
    }
    
    
    // Now create the logical device
    {
        QueueFamilyIndices indices = findQueueFamilies(this->vkPhysicalDevice);
        
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
        
        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;
        createInfo.enabledLayerCount = 0;
        
        if (vkCreateDevice(this->vkPhysicalDevice, &createInfo, nullptr, &this->vkDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }
        
        // Also get the graphics queue
        vkGetDeviceQueue(this->vkDevice, indices.graphicsFamily.value(), 0, &this->vkGraphicsQueue);
        vkGetDeviceQueue(this->vkDevice, indices.presentFamily.value(), 0, &this->vkPresentQueue);

    }
    
    
    
    
    
    std::cout << "Finished setting up Vulkan objects." << std::endl;
}

QueueFamilyIndices App::findQueueFamilies(VkPhysicalDevice device) {
    
    QueueFamilyIndices indices;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        
        VkBool32 presentSupport = false;
        
        VkSurfaceCapabilitiesKHR khrCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->vkSurface, &khrCapabilities);
        
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->vkSurface, &presentSupport);

        if (presentSupport && (khrCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
            indices.presentFamily = i;
        }
        
        if (indices.isViable()) {
            break;
        }

        i++;
    }

    return indices;
}

bool App::isDeviceSuitable(VkPhysicalDevice device) {
    
    QueueFamilyIndices indices = findQueueFamilies(device);
    
    return indices.isViable();
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
