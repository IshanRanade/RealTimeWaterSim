cmake_minimum_required(VERSION 3.8)
project(RealTimeWaterSim CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall")

# GLFW
find_package(glfw3 REQUIRED)

# Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(CPP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${CPP_SOURCES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${CPP_SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${Vulkan_LIBRARIES})
