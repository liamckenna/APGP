cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Force the use of the multi-threaded DLL runtime (/MD)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")

# Define paths
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIBS_DIR "${CMAKE_SOURCE_DIR}/libs")

# Set output directory for compiled binaries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Add include directories
include_directories(${INCLUDE_DIR})
include_directories(${LIBS_DIR})
include_directories("C:/ProgramData/chocolatey/include")
include_directories("C:/Users/lmcmc/vcpkg/installed/x86-windows/include")
include_directories("W:/Libs/glew-2.1.0-win32/glew-2.1.0/include")
# Add library directories
link_directories("C:/ProgramData/chocolatey/lib/glfw3/tools/glfw-3.0.4.bin.WIN64/lib-msvc120")
link_directories("W:/Libs/glew-2.1.0-win32/glew-2.1.0/lib/Release/x64")
# Gather all .cpp files in src directory
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# Add the executable target
add_executable(epic ${SOURCES})

# Link external libraries
target_link_libraries(epic
    glew32
    glfw3
    gdi32
    opengl32
	legacy_stdio_definitions  # Added for resolving sscanf and vsnprintf
)
