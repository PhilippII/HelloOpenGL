cmake_minimum_required(VERSION 3.11)
# CMP0072 -policy regarding find_package(OpenGL ...)
# is new in version 3.11

project(HelloOpenGL)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(OpenGLDemos
	src/Camera.cxx
	src/colorspace_utils.cxx
	src/ControllerCamera.cxx
	src/ControllerCameraStepped.cxx
	src/ControllerSun.cxx
	src/cpu_mesh_generate.cxx
	src/cpu_mesh_import.cxx
	src/cpu_mesh_structs.cxx
	src/cpu_mesh_utils.cxx
	src/debug_utils.cxx
	src/GLBufferObject.cxx
	src/GLIndexBuffer.cxx
	src/GLRenderer.cxx
	src/GLShader.cxx
	src/GLShaderProgram.cxx
	src/GLTexture.cxx
	src/GLVertexArray.cxx
	src/GLVertexBuffer.cxx
	src/main.cxx
	src/VertexBufferLayout.cxx
	src/demos/DemoClearColor.cxx
	src/demos/Demo.cxx
	src/demos/DemoLinearColorspace.cxx
	src/demos/DemoLoadOBJ.cxx
	src/demos/DemoMultipleConcepts.cxx
	src/demos/DemoPhongReflectionModel.cxx
	src/demos/DemoPhongReflectionModelTextured.cxx
)

target_include_directories(OpenGLDemos PUBLIC
                           inc
                           inc/demos)


find_package(glfw3 REQUIRED)
# find_package(glfw3 3.3 REQUIRED)
target_link_libraries(OpenGLDemos PUBLIC glfw)

find_package(GLEW REQUIRED)
target_link_libraries(OpenGLDemos PUBLIC GLEW::GLEW)

find_package(OpenGL REQUIRED)
target_link_libraries(OpenGLDemos PUBLIC OpenGL::GL)


set(VENDOR_DIR "3rd_party")

add_subdirectory("${VENDOR_DIR}/glm")
target_link_libraries(OpenGLDemos PUBLIC GLM)

add_subdirectory("${VENDOR_DIR}/dear_imgui/imgui")
target_link_libraries(OpenGLDemos PUBLIC DearImGui_glfw_opengl3)

#add_subdirectory("${VENDOR_DIR}/GSL-master")
add_library(GSL INTERFACE)
target_include_directories(GSL INTERFACE
                           "${VENDOR_DIR}/GSL-master/include")
target_link_libraries(OpenGLDemos PUBLIC GSL)

add_subdirectory("${VENDOR_DIR}/stb_image")
target_link_libraries(OpenGLDemos PUBLIC stb_image)



