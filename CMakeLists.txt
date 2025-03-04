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
    include
)

# Gather source files excluding stb.cpp
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE INCLUDESOURCES include/*.cpp include/*.c)

# Add the executable
add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${INCLUDESOURCES}
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/glfw3.lib
        ${CMAKE_SOURCE_DIR}/lib/OpenAL32.lib
        ${CMAKE_SOURCE_DIR}/lib/box2d.lib
        ${CMAKE_SOURCE_DIR}/lib/cimgui.lib
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
