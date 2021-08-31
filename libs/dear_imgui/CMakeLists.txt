add_library(dear_imgui)

target_sources(
    dear_imgui
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/backends/imgui_impl_glfw.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/backends/imgui_impl_opengl3.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/imgui.h
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/imgui_widgets.cpp
)

target_include_directories(
    dear_imgui
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui
)

target_link_libraries(
    dear_imgui
PRIVATE
    glfw
)   
