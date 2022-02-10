################################################################################
################################################################################
################################################################################
# Dependencies.

################################################################################
# Conan dependencies.

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

find_package(assimp REQUIRED CONFIG)
find_package(glew REQUIRED CONFIG)
find_package(glfw3 REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(imgui REQUIRED CONFIG)
find_package(Microsoft.GSL REQUIRED CONFIG)

################################################################################
# External dependencies.

find_package(abstractgl REQUIRED abstractgl)
find_package(api_opengl REQUIRED)

################################################################################
################################################################################
################################################################################
# Hardcoded configuration badness.

if(MSVC)
    add_compile_options(/std:c++latest /W4)
elseif()
    add_compile_options(-std=c++20 -Wall)
endif()

################################################################################
################################################################################
################################################################################
# Libraries.

################################################################################
# Common.

add_library(common INTERFACE)

target_include_directories(common
    INTERFACE
        .
)

################################################################################
# All.

add_library(all_libraries INTERFACE)

target_link_libraries(all_libraries
    INTERFACE
        # Conan dependencies.
        assimp::assimp
        GLEW::glew_s
        glfw::glfw
        glm::glm
        imgui::imgui
        Microsoft.GSL::GSL

        # External dependencies.
        abstractgl::abstractgl
        abstractgl::api_opengl

        # Internal depedencies.
        imgui_impl_glfw
        imgui_impl_opengl3

        common
)

################################################################################
################################################################################
################################################################################
# Executables.

################################################################################
# hello_triangle.

add_executable(hello_triangle)

target_link_libraries(hello_triangle
    PRIVATE
        all_libraries
)

target_sources(hello_triangle
    PRIVATE
        app/hello_triangle/main.cpp
)

################################################################################
# imgui_demo.

add_executable(imgui_demo)

target_link_libraries(imgui_demo
    PRIVATE
        all_libraries
)

target_sources(imgui_demo
    PRIVATE
        app/imgui_demo/main.cpp
)
