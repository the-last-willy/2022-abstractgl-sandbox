cmake_minimum_required(VERSION 3.15.0)
project(sandbox VERSION 0.1.0)

find_package(glfw3 3.3 REQUIRED)

if(MSVC)
    add_compile_options(/std:c++latest /W4)
endif()

# Librabry 'glad'.

add_library(glad libs/glad/src/glad.c)

target_include_directories(glad
PUBLIC
    libs/glad/include
)

# Librabry 'common'.

add_library(common INTERFACE)

target_include_directories(common
INTERFACE
    libs/stb
    libs/tiny_obj_loader-1.0.7
    src
)

target_link_libraries(common
INTERFACE
    glad
    glfw
)

# Executable 'audi'

add_executable(audi src/sandbox/audi.cpp)
target_link_libraries(audi PRIVATE common)

# Executable 'lookat'

add_executable(lookat src/sandbox/lookat.cpp)
target_link_libraries(lookat PRIVATE common)

# Executable 'terrain'

add_executable(terrain src/sandbox/terrain.cpp)
target_link_libraries(terrain PRIVATE common)

# Executable 'test'

add_executable(test src/sandbox/test.cpp)
target_link_libraries(test PRIVATE common)
