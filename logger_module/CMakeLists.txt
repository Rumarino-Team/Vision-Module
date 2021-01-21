# Set the project name
project (logger_module)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/loggermod.cpp)
add_library(mod::logger ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# Required libs for linking
target_link_libraries(${PROJECT_NAME})