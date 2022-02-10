cmake_minimum_required(VERSION 3.15.0)
project(abstractgl_sandbox VERSION 0.1.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(ext/imgui)

add_subdirectory(src)
