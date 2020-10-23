# NOTE: The whole project is meant for linux only
# NOTE: Install both ZED SDK and DarkHelp
cmake_minimum_required (VERSION 3.5)

project(vision)

# Setup googletest at config time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# Variables scopes follow standard rules
# Variables defined here will carry over to its children, ergo subdirectories

# Setup ZED libs
find_package(ZED 3 REQUIRED)
include_directories(${ZED_INCLUDE_DIRS})
link_directories(${ZED_LIBRARY_DIR})

# Setup CUDA libs for zed and ai modules
find_package(CUDA ${ZED_CUDA_VERSION} EXACT REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})
link_directories(${CUDA_LIBRARY_DIRS})

# Setup OpenCV libs
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INLCUDE_DIRS})

# Setup Darknet libs
find_library(DARKHELP_LIBRARY NAMES darkhelp libdarkhelp.so)
find_library(DARKNET_LIBRARY NAMES darknet libdarknet.so)

# System libs
SET(SPECIAL_OS_LIBS "pthread")

# Optional definitions
add_definitions(-std=c++14 -g -O3)

# Add sub directories
add_subdirectory(zed_module)
add_subdirectory(ai_module)
add_subdirectory(api_module)
add_subdirectory(executable_module)