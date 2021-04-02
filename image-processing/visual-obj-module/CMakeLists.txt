# Set the project name
project (visual_obj_module)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/visualobjmod.cpp)
add_library(mod::visual-obj-mod ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# Required libs for linking
target_link_libraries(${PROJECT_NAME}
        mod::zed
        mod::pipeline-ai
        mod::logger
        ${SPECIAL_OS_LIBS}
        ${OpenCV_LIBS})