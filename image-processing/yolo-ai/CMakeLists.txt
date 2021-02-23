# Set the project name
project (yolo_module)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/yolomod.cpp)
add_library(mod::yolo-ai ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# Required libs for linking
target_link_libraries(${PROJECT_NAME}
                        mod::zed
                        ${SPECIAL_OS_LIBS}
                        ${OpenCV_LIBS}
                        ${DARKNET_LIBRARY}
                        #${DARK_LIBS}
                        ${CUDA_LIBRARIES} ${CUDA_npps_LIBRARY})