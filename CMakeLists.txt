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
    include/bgfx/
)

# Gather source files excluding stb.cpp
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE INCLUDESOURCES 
    include/*.cpp 
    include/*.c
)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_compile_options(
    /Zc:__cplusplus
    /Zc:preprocessor
)

add_definitions(
    -DBX_CONFIG_DEBUG=1
)

# Add the executable
add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${INCLUDESOURCES}
)

# Link libraries for the main project
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/glfw3.lib
        ${CMAKE_SOURCE_DIR}/lib/OpenAL32.lib
        ${CMAKE_SOURCE_DIR}/lib/box2d.lib
        ${CMAKE_SOURCE_DIR}/lib/assimp-vc143-mt.lib
        ${CMAKE_SOURCE_DIR}/lib/joltc_double.lib
        ${CMAKE_SOURCE_DIR}/lib/bgfxRelease.lib
        ${CMAKE_SOURCE_DIR}/lib/bimg_decodeRelease.lib
        ${CMAKE_SOURCE_DIR}/lib/bxRelease.lib
        ${CMAKE_SOURCE_DIR}/lib/bimgRelease.lib
        opengl32
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/glfw3.a
        ${CMAKE_SOURCE_DIR}/lib/openal.so
        ${CMAKE_SOURCE_DIR}/lib/box2d.a
        GL
    )
endif()
