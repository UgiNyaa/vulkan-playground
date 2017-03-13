cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-Wall")

project(vulkan-playground)

file(GLOB_RECURSE vpg_SOURCES_HEADERS "src/*.cpp" "include/*.hpp")

include_directories(include)
add_executable(vulkan-playground ${vpg_SOURCES_HEADERS})
target_link_libraries(vulkan-playground vulkan xcb)
