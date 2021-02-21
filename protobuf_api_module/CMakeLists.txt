# Set the project name
project (protobuf_api_module)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/apimod.cpp src/model.pb.cc)
add_library(mod::protobufapi ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# Required libs for linking
#TODO MAYBE DELETE PROTOBUF LIBRARY
target_link_libraries(${PROJECT_NAME}
        ${Protobuf_LIBRARIES}
        mod::zed
        mod::ai)

