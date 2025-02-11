#include "glew_init.h"
#include <GL/glew.h>
#include <iostream>

void InitializeGLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "GLEW initialized" << std::endl;
}