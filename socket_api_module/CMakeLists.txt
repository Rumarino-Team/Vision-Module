# Set the project name
#project (api_module)

# Add a library with the above sources
#add_library(api_module src/api_module.cpp)
#add_library(mod::api ALIAS api_module)

#target_include_directories( api_module
#    PUBLIC ${PROJECT_SOURCE_DIR}/include
#)


# Set the project name
project (apimod)

# Make sure PyBind11 is installed and Python3 is found
#find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(PythonLibs 3)
find_package(pybind11 CONFIG REQUIRED)

# Build target python module
pybind11_add_module(${PROJECT_NAME} NO_EXTRAS src/apimod.cpp)

# Required libs for linking
#target_link_libraries(${PROJECT_NAME} PRIVATE mod::zed mod::ai)

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Create the executable
#add_executable(${PROJECT_NAME} src/server.cpp)

#target_include_directories( ${PROJECT_NAME}
#    PUBLIC ${PROJECT_SOURCE_DIR}/include
#)

#target_link_libraries(${PROJECT_NAME}
#    mod::zed
#    mod::ai
#)