# NOTE: The whole project is meant for linux only
# NOTE: Install both ZED SDK and DarkHelp
cmake_minimum_required (VERSION 3.5)

project(vision)

# Editable variables
SET(VERSION_REQ_CUDA "10.2")


# Variables scopes follow standard rules
# Variables defined here will carry over to its children, ergo subdirectories

# Setup CUDA libs for zed and ai modules
find_package(CUDA ${VERSION_REQ_CUDA} REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(${CUDA_LIBRARY_DIRS})

# Setup ZED libs
find_package(ZED 2 REQUIRED)
include_directories(${ZED_INCLUDE_DIRS})
link_directories(${ZED_LIBRARY_DIR})

# Setup Darknet libs
find_library(DARKHELP_LIBRARY NAMES darkhelp libdarkhelp.so)
find_library(DARKNET_LIBRARY NAMES darknet libdarknet.so)

# System libs
SET(SPECIAL_OS_LIBS "pthread")

# Optional definitions
add_definitions(-std=c++0x -g -O3)

# Add sub directories
add_subdirectory(zed_module)
add_subdirectory(ai_module)
add_subdirectory(api_module)
add_subdirectory(executable_module)