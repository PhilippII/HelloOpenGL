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



