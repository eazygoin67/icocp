# icocp
# Target: icocp_app

file(
    GLOB_RECURSE 
    APP_HEADER_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR} *.h
)

file(
    GLOB_RECURSE 
    APP_SOURCE_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR} *.c
)

set(APP_ALL_SOURCE_FILES
    ${APP_HEADER_FILES}
    ${APP_SOURCE_FILES}
)

add_executable(icocp_app 
    ${APP_ALL_SOURCE_FILES}
)

target_include_directories(icocp_app PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(icocp_app PUBLIC 
    stb
)

set_property(
    TARGET icocp_app 
    PROPERTY VS_DEBUGGER_WORKING_DIRECTORY 
    "${CMAKE_SOURCE_DIR}"
)