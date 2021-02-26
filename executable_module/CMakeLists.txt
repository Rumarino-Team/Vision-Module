project(vision_http_server)

# Create the executable
add_executable(${PROJECT_NAME} main.cpp)

# This will cause the include directories for that target to be added to this project
target_link_libraries(${PROJECT_NAME}
    mod::zed
    mod::yolo-ai
    mod::http-api
)