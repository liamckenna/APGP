#include "glfw_setup.h"
#include <iostream>
#include "json.h"
void InitializeGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "GLFW initialized" << std::endl;
}
