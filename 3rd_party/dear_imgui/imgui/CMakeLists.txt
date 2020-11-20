add_library(DearImGui_glfw_opengl3 STATIC
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
        bindings/imgui_impl_glfw.cpp
    bindings/imgui_impl_opengl3.cpp	
)

# specify headers as system headers in order to skip warinings
target_include_directories(DearImGui_glfw_opengl3 SYSTEM
                           PUBLIC . bindings)

# By using the aliases we avoid having to make a case distinction
# whether glfw and glew were imported manually or via find_package().
# Also this allows us to control from the main CMakeLists.txt-file 
# which exact version of glfw and glew we want to use:
target_link_libraries(DearImGui_glfw_opengl3
                        PRIVATE glfw_alias
                        PRIVATE glew_alias
                        PRIVATE OpenGL::GL)

