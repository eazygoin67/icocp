# stb
# Target: stb

file(
    GLOB_RECURSE 
    STB_HEADER_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib *.h
)

set(STB_ALL_SOURCE_FILES
    ${STB_HEADER_FILES}
    stb_lib.c
)

add_library(stb ${STB_ALL_SOURCE_FILES})

target_include_directories(stb PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)