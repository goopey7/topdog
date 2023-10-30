cmake_minimum_required(VERSION 3.12)
project(cmp303)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Recursively glob all .cpp and .h files in src/ directory
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.h")

# Define the C++ executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# definitions
# target_compile_definitions(${PROJECT_NAME} PRIVATE RENDERER_VULKAN=1)

find_package(RAYLIB REQUIRED)
include_directories(${RAYLIB_INCLUDE_DIR})

# Link the C++ executable
target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIBRARY})
