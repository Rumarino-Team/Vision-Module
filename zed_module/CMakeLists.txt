# Set the project name
project (api_module)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/apimod.cpp)
add_library(mod::api ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)