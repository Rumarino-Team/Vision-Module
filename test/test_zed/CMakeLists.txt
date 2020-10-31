file(GLOB SRCS *.cpp)

project (test_zed)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    libgtest
    mod::zed
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

# Copy resource directories when compiling
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/media ${CMAKE_BINARY_DIR}/test/test_zed/media)