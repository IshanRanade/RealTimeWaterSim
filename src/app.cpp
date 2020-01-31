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
    window = glfwCreateWindow(this->width, this->height, "Real Time Water Simulation", NULL, NULL);
    
    // Setup Vulkan
    if (!glfwVulkanSupported())
    {
        throw std::runtime_error("GLFW: Vulkan Not Supported");
    }
    
    // Setup all the vulkan members
    setupVulkan();
}

App::~App() {
    
    for (auto imageView : this->swapChainImageViews) {
        vkDestroyImageView(this->vkDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(this->vkDevice, this->vkSwapChain, nullptr);
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
        
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        
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
        
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        
        createInfo.enabledLayerCount = 0;
        
        if (vkCreateDevice(this->vkPhysicalDevice, &createInfo, nullptr, &this->vkDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }
        
        // Get the graphics and present queues
        vkGetDeviceQueue(this->vkDevice, indices.graphicsFamily.value(), 0, &this->vkGraphicsQueue);
        vkGetDeviceQueue(this->vkDevice, indices.presentFamily.value(), 0, &this->vkPresentQueue);
    }
    
    
    // Now create the swap chain
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(this->vkPhysicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
        
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = this->vkSurface;
        
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        
        QueueFamilyIndices indices = findQueueFamilies(this->vkPhysicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        
        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(this->vkDevice, &createInfo, nullptr, &this->vkSwapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }
        
        vkGetSwapchainImagesKHR(this->vkDevice, this->vkSwapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(this->vkDevice, this->vkSwapChain, &imageCount, this->swapChainImages.data());

        this->swapChainImageFormat = surfaceFormat.format;
        this->swapChainExtent = extent;
    }
    
    
    // Now create the image views
    {
        this->swapChainImageViews.resize(this->swapChainImages.size());
        
        for (size_t i = 0; i < this->swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            
            if (vkCreateImageView(this->vkDevice, &createInfo, nullptr, &this->swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }

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
    
    bool extensionsSupported = checkDeviceExtensionSupport(device);
    
    bool swapChainAdequate = false;
    if (extensionsSupported) {
      SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
      swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    
    return indices.isViable() && extensionsSupported && swapChainAdequate;
}

bool App::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(this->deviceExtensions.begin(), this->deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails App::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->vkSurface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->vkSurface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->vkSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->vkSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->vkSurface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR App::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR App::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D App::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {this->width, this->height};

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

void App::createVKBufferWithPixelData(VkCommandPool commandPool) {
    
    // Create a new command buffer to fill a vkbuffer with pixel data
    std::vector<uint8_t> pixels;
    pixels.resize(this->width * this->height * 4);
    for(int i = 0; i < pixels.size(); i+=4) {
        pixels[i]   = 255;
        pixels[i+1] = 0;
        pixels[i+2] = 0;
        pixels[i+3] = 255;
    }
    
    VkCommandBuffer commandBuffer;
    
    // Create the command buffer
    VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
    cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocInfo.commandPool = commandPool;
    cmdBufAllocInfo.commandBufferCount = 1;
    cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    
    if(vkAllocateCommandBuffers(this->vkDevice, &cmdBufAllocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command buffers");
    }
    
    QueueFamilyIndices indices = findQueueFamilies(this->vkPhysicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    VkBufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = NULL;
    bufferCreateInfo.size = this->width * this->height * 4;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    if(vkCreateBuffer(this->vkDevice, &bufferCreateInfo, NULL, &this->pixelDataBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vkbuffer");
    }
    
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(this->vkDevice, this->pixelDataBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    VkMemoryPropertyFlags properties;
    //allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
    
    
    VkDeviceMemory bufferMemory;

    if (vkAllocateMemory(this->vkDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(this->vkDevice, this->pixelDataBuffer, bufferMemory, 0);
    
    //std::cout << this->pixelDataBuffer << std::endl;
    
    std::vector<VkFence> inFlightFences;
    inFlightFences.resize(1);
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if(vkCreateFence(this->vkDevice, &fenceInfo, nullptr, &inFlightFences[0]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence");
    }
    
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    
    if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("error 1");
    }
    
    vkCmdUpdateBuffer(commandBuffer, this->pixelDataBuffer, 0, this->height * this->width * 4, pixels.data());
    
    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("error 2");
    }
    
    VkSubmitInfo submitInfo = {};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &commandBuffer;
    
    if(vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, inFlightFences[0]) != VK_SUCCESS) {
        throw std::runtime_error("error 4");
    }
    
    vkWaitForFences(this->vkDevice, 1, &inFlightFences[0], VK_TRUE, UINT64_MAX);
    vkResetFences(this->vkDevice, 1, &inFlightFences[0]);
    
    
    
    
    
    
    
    
    {
        VkCommandBuffer commandBuffer;

        VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
        cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdBufAllocInfo.commandPool = commandPool;
        cmdBufAllocInfo.commandBufferCount = 1;
        cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        
        if(vkAllocateCommandBuffers(this->vkDevice, &cmdBufAllocInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command buffers");
        }
        
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        VkBufferImageCopy vkBufferImageCopy;
        vkBufferImageCopy.bufferOffset = 0;
        vkBufferImageCopy.bufferRowLength = 0;
        vkBufferImageCopy.bufferImageHeight = 0;
        
        vkBufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vkBufferImageCopy.imageSubresource.mipLevel = 0;
        vkBufferImageCopy.imageSubresource.baseArrayLayer = 0;
        vkBufferImageCopy.imageSubresource.layerCount = 1;

        vkBufferImageCopy.imageOffset = {0, 0, 0};
        vkBufferImageCopy.imageExtent = {
            width,
            height,
            1
        };
        
        VkSubmitInfo submitInfo = {};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &commandBuffer;
        
        
        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("error 1");
        }
        
        vkCmdCopyBufferToImage(commandBuffer, this->pixelDataBuffer, this->swapChainImages[0], VK_IMAGE_LAYOUT_GENERAL, 1, &vkBufferImageCopy);

        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
         throw std::runtime_error("error 2");
        }
        
        if(vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, inFlightFences[0]) != VK_SUCCESS) {
            throw std::runtime_error("error 4");
        }
        vkWaitForFences(this->vkDevice, 1, &inFlightFences[0], VK_TRUE, UINT64_MAX);
        vkResetFences(this->vkDevice, 1, &inFlightFences[0]);
        
        
        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("error 1");
        }
        
        vkCmdCopyBufferToImage(commandBuffer, this->pixelDataBuffer, this->swapChainImages[1], VK_IMAGE_LAYOUT_GENERAL, 1, &vkBufferImageCopy);

        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
         throw std::runtime_error("error 2");
        }
        
        if(vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, inFlightFences[0]) != VK_SUCCESS) {
            throw std::runtime_error("error 4");
        }
        vkWaitForFences(this->vkDevice, 1, &inFlightFences[0], VK_TRUE, UINT64_MAX);
        vkResetFences(this->vkDevice, 1, &inFlightFences[0]);
        
        
        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("error 1");
        }
        
        vkCmdCopyBufferToImage(commandBuffer, this->pixelDataBuffer, this->swapChainImages[2], VK_IMAGE_LAYOUT_GENERAL, 1, &vkBufferImageCopy);

        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
         throw std::runtime_error("error 2");
        }
        
        if(vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, inFlightFences[0]) != VK_SUCCESS) {
            throw std::runtime_error("error 4");
        }
        vkWaitForFences(this->vkDevice, 1, &inFlightFences[0], VK_TRUE, UINT64_MAX);
        vkResetFences(this->vkDevice, 1, &inFlightFences[0]);
    }
    
    
}


int App::start() {
    
    std::cout << "Showing display." << std::endl;
        
    // Create the command pool
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(this->vkPhysicalDevice);
    
    VkCommandPool vkCommandPool;
    
    VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
    cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    
    if(vkCreateCommandPool(this->vkDevice, &cmdPoolCreateInfo, NULL, &vkCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool");
    }
    
    //copyBufferToImages(vkCommandPool);
    this->createVKBufferWithPixelData(vkCommandPool);
    
    
  
    
    
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(this->vkDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(this->vkDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(this->vkDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
    


    // Display the GUI and main loop
    //int times = 0;
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        vkWaitForFences(this->vkDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        
        uint imageIndex = 0;
        if(vkAcquireNextImageKHR(this->vkDevice, this->vkSwapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS) {
            throw std::runtime_error("error 3");
        }
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];

        
        std::cout << imageIndex << std::endl;
        
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = &this->vkSwapChain;
        presentInfo.pImageIndices      = &imageIndex;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
        
        if(vkQueuePresentKHR(this->vkPresentQueue, &presentInfo) != VK_SUCCESS) {
            throw std::runtime_error("error 5");
        }
        vkQueueWaitIdle(this->vkPresentQueue);

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
