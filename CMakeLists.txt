cmake_minimum_required(VERSION 3.15.0)
project(sandbox VERSION 0.1.0)

add_subdirectory(libs/abstractgl)
add_subdirectory(libs/dear_imgui)
add_subdirectory(libs/glad)
add_subdirectory(libs/glfw3)
add_subdirectory(libs/tinygltf)

if(MSVC)
    add_compile_options(/std:c++latest /W4)
elseif()
    add_compile_options(/std=c++20 /Wall)
endif()

# Library 'common'.

add_library(common INTERFACE)

target_include_directories(common
INTERFACE
    src
)

target_link_libraries(common
INTERFACE
    abstractgl
    dear_imgui
    glad
    glfw
    tinygltf
)

# Executable 'chaine'.

add_executable(chaine src/chaine/main.cpp)
target_link_libraries(chaine PRIVATE common)

# Executable 'dear_imgui'

add_executable(dear_imgui_demo src/sandbox/dear_imgui.cpp)
target_link_libraries(dear_imgui_demo PRIVATE common)


# Executable 'gltf'

add_executable(gltf src/sandbox/gltf.cpp)
target_link_libraries(gltf PRIVATE common)
