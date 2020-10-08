# Set the project name
project (ai_module)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/aimod.cpp)
add_library(mod::ai ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)