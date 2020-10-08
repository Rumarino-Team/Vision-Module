project(executable_module)

# Create the executable
add_executable(${PROJECT_NAME} main.cpp)

# Link the static library from subproject1 using it's alias
# This will cause the include directories for that target to be added to this project
target_link_libraries(${PROJECT_NAME}
    mod::zed
    mod::ai
    mod::api
)