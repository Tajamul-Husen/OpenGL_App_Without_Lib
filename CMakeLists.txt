cmake_minimum_required(VERSION VERSION 3.22.1)

project(OPENGL_NO_LIB_APP VERSION 1.0.0)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_CXX_STANDARD 17)

file (
    GLOB_RECURSE SOURCES_LIST 
    ${PROJECT_SOURCE_DIR}/src/*.cpp; 
    ${PROJECT_SOURCE_DIR}/src/**/*.cpp;
)

# OpenGL
find_package (OpenGL REQUIRED)
if (OPENGL_FOUND)
    message ("-- OpenGL found.")
else (OPENGL_FOUND)
    message ("-- OpenGL not found.")
endif ()


set (INCLUDE_DIR 
    ${PROJECT_SOURCE_DIR}/src
)

# add_executable(${PROJECT_NAME} WIN32 ${SOURCES_LIST})
add_executable(${PROJECT_NAME} ${SOURCES_LIST})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})

target_compile_definitions(
    ${PROJECT_NAME} 
    PUBLIC 
    "$<$<CONFIG:DEBUG>:OPENGL_NO_LIB_APP_DEBUG>"
    "$<$<CONFIG:Release>:OPENGL_NO_LIB_APP_RELEASE>"
)

