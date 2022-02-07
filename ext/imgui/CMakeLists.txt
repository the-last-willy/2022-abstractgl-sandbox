################################################################################

add_library(imgui_impl_glfw)

target_include_directories(imgui_impl_glfw
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)

target_sources(imgui_impl_glfw
    PRIVATE
        imgui/imgui_impl_glfw.cpp
    PUBLIC
        imgui/imgui_impl_glfw.h
)

################################################################################

add_library(imgui_impl_opengl3)

target_include_directories(imgui_impl_opengl3
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)

target_sources(imgui_impl_opengl3
    PRIVATE
        imgui/imgui_impl_opengl3.cpp
    PUBLIC
        imgui/imgui_impl_opengl3.h
        imgui/imgui_impl_opengl3_loader.h
)
