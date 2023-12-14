# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)

project(imgui_lib)

file(GLOB SRC_FILES "*.cpp" "*.h" "extras/*.cpp" "extras/*.h")

add_library(${PROJECT_NAME} ${SRC_FILES})
