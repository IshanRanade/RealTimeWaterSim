#include "app.h"

App::App() {
    // Setup GLFW window
    if (!glfwInit()) {
        throw std::runtime_error("Could not initialize glfw");
    }
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(1280, 720, "Real Time Water Simulation", NULL, NULL);
}

int App::start() {
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
