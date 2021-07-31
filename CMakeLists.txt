cmake_minimum_required(VERSION 3.15.0)
project(sandbox VERSION 0.1.0)

add_subdirectory(
    "D:/dev/project/abstractgl"
    "D:/dev/project/abstractgl/build")

# find_package(AbstractGL REQUIRED)

find_package(glfw3 3.3 REQUIRED)

if(MSVC)
    add_compile_options(/std:c++latest /W4)
endif()

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
    AbstractGL
    glfw
)

# Executable 'lsystem'

add_executable(lsystem src/sandbox/lsystem.cpp)
target_link_libraries(lsystem PRIVATE common)

# Executable 'terrain'

add_executable(terrain src/sandbox/terrain.cpp)
target_link_libraries(terrain PRIVATE common)

# Executable 'tree'

add_executable(tree src/sandbox/tree.cpp)
target_link_libraries(tree PRIVATE common)

# Executable 'tree'

add_executable(tree_traversal src/sandbox/tree_traversal.cpp)
target_link_libraries(tree_traversal PRIVATE common)
