# Set the project name
project (zed_module)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/zedmod.cpp)
add_library(mod::zed ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)