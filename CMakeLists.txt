cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(VCPKG_ROOT "C:/Users/lmcmc/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set vcpkg path for dependencies
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};C:/Users/lmcmc/vcpkg/installed/x64-windows")
set(CMAKE_FIND_DEBUG_MODE TRUE)

# Use multi-threaded DLL runtime (/MD)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")

# Define paths
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIBS_DIR "${CMAKE_SOURCE_DIR}/libs")  # Directory for json.hpp and stb_image.h

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Gather all .cpp files in src directory
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# Add the executable target
add_executable(epic ${SOURCES})

# Link include directories for target
target_include_directories(epic PRIVATE ${INCLUDE_DIR} ${LIBS_DIR})

# Attempt to find and link GLFW using vcpkg
find_package(glfw3 CONFIG REQUIRED)
if(glfw3_FOUND)
    message(STATUS "GLFW found at: ${GLFW3_DIR}")
    target_link_libraries(epic PRIVATE glfw)
else()
    message(FATAL_ERROR "GLFW not found! Please install GLFW (e.g., via vcpkg) and try again.")
endif()

# Attempt to find and link GLEW using vcpkg
find_package(glew CONFIG REQUIRED PATHS "C:/Users/lmcmc/vcpkg/installed/x64-windows")
if(TARGET GLEW::GLEW)
    target_link_libraries(epic PRIVATE GLEW::GLEW)
else()
    message(FATAL_ERROR "GLEW not found! Please install GLEW (e.g., via vcpkg) and try again.")
endif()

# Link additional Windows-specific libraries
target_link_libraries(epic PRIVATE
    gdi32
    opengl32
    legacy_stdio_definitions  # For resolving sscanf and vsnprintf
)
