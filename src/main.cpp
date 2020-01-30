#include <iostream>
#include "app.h"


App *app;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(app->window, GL_TRUE);
    }
}

int main() {
    std::cout << "Starting program..." << std::endl;
    
    std::cout << __VERSION__ << std::endl;
    
    app = new App();
    
    glfwSetKeyCallback(app->window, keyCallback);

    int result = app->start();
    return result;
}
