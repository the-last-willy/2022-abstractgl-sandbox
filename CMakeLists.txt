cmake_minimum_required(VERSION 3.15.0)
project(sandbox VERSION 0.1.0)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw-3.3.4)

if(MSVC)
    add_compile_options(/std:c++latest /W4)
endif()

# Executable 'audi'

add_executable(audi)

target_sources(audi
PRIVATE
    libs/glad/src/glad.c
    src/sandbox/audi.cpp
)

target_include_directories(audi
PRIVATE
    libs/glad/include
    libs/glfw-3.3.4/deps
    libs/stb
    libs/tiny_obj_loader-1.0.7
    src
)

target_link_libraries(audi
PRIVATE
    glfw
)

# Executable 'setup'

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
    libs/stb
    libs/tiny_obj_loader-1.0.7
    src
)

target_link_libraries(setup
PRIVATE
    glfw
)
