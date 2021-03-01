# Set the project name
project (pipeline_module)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/pipelinemod.cpp)
add_library(mod::pipeline-ai ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# Required libs for linking
target_link_libraries(${PROJECT_NAME}
                        mod::zed
                        ${SPECIAL_OS_LIBS}
                        ${OpenCV_LIBS})