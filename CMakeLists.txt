# icocp
# MAIN CMAKE FILE

set(CMAKE_CONFIGURATION_TYPES 
    Debug
    Testing
    Release
)

SET(PROJECT_NAME icocp)
message("Using project name: ${PROJECT_NAME}")

cmake_minimum_required(VERSION 3.23.0)
project(${PROJECT_NAME})

set(ICOCP_GLOBAL_CONFIG_DEBUG Debug)
set(ICOCP_GLOBAL_CONFIG_TESTING Testing)
set(ICOCP_GLOBAL_CONFIG_RELEASE Release)

include(cmake/common_definitions.cmake)

set(PROJECT_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/)
set(PRODUCTION_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)
set(CMAKE_OBJECT_PATH_MAX 300)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_compile_definitions(${ICOCP_GLOBAL_DEFINITIONS})
add_compile_options("$<$<COMPILE_LANGUAGE:C,CXX>:${ICOCP_GLOBAL_OPTIONS}>")

add_link_options(${ICOCP_GLOBAL_LINK_OPTIONS})

set(CMAKE_FOLDER extlib)
add_subdirectory(src/extlib/stb)

# NOTE: Specify warning levels after external libraries (extlib) so 
# that the build step doesn't fall on it's face
add_compile_options(
    "$<$<COMPILE_LANGUAGE:C,CXX>:${ICOCP_GLOBAL_WARNING_LEVELS}>"
)

set(CMAKE_FOLDER icocp_app)
add_subdirectory(src/icocp_app)

set_property(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
    PROPERTY VS_STARTUP_PROJECT 
    icocp_app
)

set(SOLUTION_FILES 
    ${CMAKE_SOURCE_DIR}/CMakeLists.txt
    ${CMAKE_SOURCE_DIR}/cmake/common_definitions.cmake
)

add_custom_target(GlobalFiles SOURCES ${SOLUTION_FILES})