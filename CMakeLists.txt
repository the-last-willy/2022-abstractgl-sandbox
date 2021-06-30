cmake_minimum_required(VERSION 3.15.0)
project(setup VERSION 0.1.0)

add_subdirectory(libs/glfw-3.3.4)

if(MSVC)
    add_compile_options(/std:c++latest /W4)
endif()

add_executable(setup)

target_sources(setup
PRIVATE
    libs/glad/src/glad.c
    src/main.cpp
)

target_include_directories(setup
PRIVATE
    libs/glad/include
    libs/glfw-3.3.4/deps
    src
)

target_link_libraries(setup
PRIVATE
    glfw
)
