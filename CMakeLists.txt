cmake_minimum_required(VERSION 3.15.0)
project(sandbox VERSION 0.1.0)

add_subdirectory(
    "D:/dev/project/abstractgl"
    "D:/dev/project/abstractgl/build")

# find_package(AbstractGL REQUIRED)

add_subdirectory(libs/glfw3)
add_subdirectory(libs/dear_imgui)

if(MSVC)
    add_compile_options(/std:c++latest /W4)
endif()

# Library 'common'.

add_library(common INTERFACE)

target_include_directories(common
INTERFACE
    libs/stb
    libs/tiny_obj_loader-1.0.7
    libs/tinygltf
    src
)

target_link_libraries(common
INTERFACE
    AbstractGL
    dear_imgui
    glfw
)

# Executable 'dear_imgui'

add_executable(dear_imgui_demo src/sandbox/dear_imgui.cpp)
target_link_libraries(dear_imgui_demo PRIVATE common)


# Executable 'gltf'

add_executable(gltf src/sandbox/gltf.cpp)
target_link_libraries(gltf PRIVATE common)
