# Set the project name
project (http-api_module)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/apimod.cpp)
add_library(mod::http-api ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# Required libs for linking
target_link_libraries(${PROJECT_NAME}
        mod::ai
        mod::logger
        httplib::httplib
        nlohmann_json::nlohmann_json)