# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Set the project name
project(Salamander)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_STANDARD 17)

include_directories(
    include/
    ./
    cimgui/
    cimgui/imgui/
    cimgui/imgui/backends/
)

# Gather source files excluding stb.cpp
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE INCLUDESOURCES 
    include/*.cpp 
    include/*.c
    EXCLUDE 
    include/cimgui/*
)

# Add the executable
add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${INCLUDESOURCES}
)

set(CIMGUI_DIR ${CMAKE_SOURCE_DIR}/cimgui)
set(CIMGUI_IMGUI ${CIMGUI_DIR}/imgui)
set(CIMGUI_IMGUI_IMPL ${CIMGUI_IMGUI}/backends)
set(CIMGUI_INCLUDE_DIRS
    ${CIMGUI_DIR}
    ${CIMGUI_DIR}/generator/output
    ${CIMGUI_IMGUI}
    ${CIMGUI_IMGUI_IMPL}
)

# Explicitly list all cimgui and imgui source files
file(GLOB CIMGUI_FILES
    ${CIMGUI_DIR}/cimgui.cpp
    ${CIMGUI_IMGUI}/*.cpp
    ${CIMGUI_IMGUI_IMPL}/imgui_impl_glfw.cpp
    ${CIMGUI_IMGUI_IMPL}/imgui_impl_opengl3.cpp
)

# Create cimgui library
add_library(cimgui STATIC ${CIMGUI_FILES})

target_include_directories(cimgui PUBLIC
    ${CIMGUI_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
)

# Compile definitions for cimgui
target_compile_definitions(cimgui PRIVATE
    IMGUI_IMPL_API=extern\ \"C\"
    IMGUI_IMPL_LOADER_GLAD
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)

# Compile definitions for the main project
target_compile_definitions(${PROJECT_NAME} PUBLIC
    CIMGUI_USE_OPENGL3
    CIMGUI_USE_GLFW
)

# Link libraries for the main project
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        cimgui
        ${CMAKE_SOURCE_DIR}/lib/glfw3.lib
        ${CMAKE_SOURCE_DIR}/lib/OpenAL32.lib
        ${CMAKE_SOURCE_DIR}/lib/box2d.lib
        opengl32
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        cimgui
        ${CMAKE_SOURCE_DIR}/lib/glfw3.a
        ${CMAKE_SOURCE_DIR}/lib/openal.so
        ${CMAKE_SOURCE_DIR}/lib/box2d.a
        GL
    )
endif()
