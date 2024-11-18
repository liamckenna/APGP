cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable verbose makefile if debugging build issues
set(CMAKE_VERBOSE_MAKEFILE ON)

# Define paths
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIBS_DIR "${CMAKE_SOURCE_DIR}/libs")  # Directory for json.hpp and stb_image.h

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)

# Gather all .cpp files in src directory
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# Add the executable target
add_executable(APGP ${SOURCES})

# Include local libraries (e.g., json.hpp, stb_image.h)
target_include_directories(APGP PRIVATE ${INCLUDE_DIR} ${LIBS_DIR})

# Find and link GLFW
find_package(glfw3 CONFIG REQUIRED)
if(glfw3_FOUND)
    message(STATUS "GLFW found at: ${GLFW3_DIR}")
    target_link_libraries(APGP PRIVATE glfw)
else()
    message(FATAL_ERROR "GLFW not found! Please install GLFW (e.g., via your package manager or vcpkg) and try again.")
endif()

# Find and link GLEW
find_package(glew CONFIG REQUIRED)
if(TARGET GLEW::GLEW)
    target_link_libraries(APGP PRIVATE GLEW::GLEW)
else()
    message(FATAL_ERROR "GLEW not found! Please install GLEW (e.g., via your package manager or vcpkg) and try again.")
endif()

# Link OpenGL and other platform-specific libraries
find_package(OpenGL REQUIRED)
target_link_libraries(APGP PRIVATE OpenGL::GL gdi32 legacy_stdio_definitions)
