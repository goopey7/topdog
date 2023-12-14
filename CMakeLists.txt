cmake_minimum_required(VERSION 3.12)
project(topdog)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

file(GLOB_RECURSE SRC_FILES "src/game/*.cpp" "src/game/*.h")

add_executable(${PROJECT_NAME} ${SRC_FILES})

find_package(RAYLIB REQUIRED)
include_directories(${RAYLIB_INCLUDE_DIR})

include_directories("libs/imgui")
add_subdirectory("libs/imgui")

target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIBRARY} imgui_lib)

project(server)
set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE SRC_FILES "src/server/*.cpp" "src/server/*.h")
add_executable(${PROJECT_NAME} ${SRC_FILES})
