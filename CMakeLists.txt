cmake_minimum_required(VERSION 3.12)
project(topdog)

set(CMAKE_CXX_STANDARD 20)

include_directories("src/common/")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

file(GLOB_RECURSE SRC_FILES "src/game/*.cpp" "src/game/*.h")

add_executable(${PROJECT_NAME} ${SRC_FILES})

include_directories("libs/imgui")
add_subdirectory("libs/imgui")
include_directories("libs/mp11/include")

add_subdirectory("libs/raylib")
include_directories("libs/raylib/src")

target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIBRARY} imgui_lib raylib)

project(server)
set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE SRC_FILES "src/server/*.cpp" "src/server/*.h")
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIBRARY})
