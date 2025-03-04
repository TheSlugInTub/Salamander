# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Set the project name
project(Salamander)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_STANDARD 17)

# Add the include directory so that it is available globally
include_directories(
    include/
    include/cimgui/
    include/cimgui/imgui/
    include/cimgui/imgui/backends/
)

# Gather source files excluding stb.cpp
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE INCLUDESOURCES 
    include/*.cpp 
    include/*.c
    EXCLUDE 
    include/cimgui/*
)

add_definitions("-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1")
set(IMGUI_LIBRARIES )

# Add the executable
add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${INCLUDESOURCES}
    ${CMAKE_SOURCE_DIR}/include/cimgui/cimgui.cpp
    ${CMAKE_SOURCE_DIR}/include/cimgui/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/include/cimgui/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/include/cimgui/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/include/cimgui/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/include/cimgui/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/include/cimgui/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/include/cimgui/imgui/backends/imgui_impl_glfw.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    IMGUI_IMPL_OPENGL_LOADER_GL3W
    CIMGUI_USE_OPENGL3 
    CIMGUI_USE_GLFW
    CIMGUI_DEFINE_ENUMS_AND_STRUCTS=1
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/glfw3.lib
        ${CMAKE_SOURCE_DIR}/lib/OpenAL32.lib
        ${CMAKE_SOURCE_DIR}/lib/box2d.lib
        ${CMAKE_SOURCE_DIR}/lib/cimgui.lib
        opengl32
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/glfw3.a
        ${CMAKE_SOURCE_DIR}/lib/openal.so
        ${CMAKE_SOURCE_DIR}/lib/box2d.a
        ${CMAKE_SOURCE_DIR}/lib/cimgui.a
    )
endif()
