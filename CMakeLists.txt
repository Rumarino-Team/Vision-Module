cmake_minimum_required (VERSION 3.5)

project(vision)

# Add sub directories
add_subdirectory(zed_module)
add_subdirectory(ai_module)
add_subdirectory(api_module)
add_subdirectory(executable_module)