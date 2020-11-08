cmake_minimum_required(VERSION 3.10)

project(HelloOpenGL)

add_executable(OpenGLDemos)

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
	src/demos
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


