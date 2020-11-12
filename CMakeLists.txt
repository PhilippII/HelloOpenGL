cmake_minimum_required(VERSION 3.11)
# CMP0072 -policy regarding find_package(OpenGL ...)
# is new in version 3.11

project(HelloOpenGL)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
add_library(warning_flags INTERFACE)
target_compile_options(warning_flags INTERFACE
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-pedantic;-Wconversion;-Wshadow;-Wcast-qual;-Wwrite-strings;-Wold-style-cast>>"
    "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
    )

add_library(debug_stl INTERFACE)
target_compile_definitions(debug_stl INTERFACE
    "$<$<CONFIG:Debug>:$<${gcc_like_cxx}:$<BUILD_INTERFACE:_GLIBCXX_DEBUG;_GLIBCXX_DEBUG_PEDANTIC>>>"
    )
# also use _GLIBCXX_CONCEPT_CHECKS ?



if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res SYMBOLIC)
endif()


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

        # hopefully cmake understands by the file suffix that the
        # following files should only be shown in qt-creator's outliner
        # but not compiled:
        res/shaders/TexturedPhongRefl.shader
)

target_include_directories(OpenGLDemos PUBLIC
                           inc
                           inc/demos
                           )

target_link_libraries(OpenGLDemos PUBLIC warning_flags)
target_link_libraries(OpenGLDemos PUBLIC debug_stl)

if(WIN32)
    add_library(glfw SHARED IMPORTED)
    set(GLFW_WIN_DIR "${PROJECT_SOURCE_DIR}/3rd_party/windows/glfw-3.3.2.bin.WIN64")
    # TODO: select correct folder lib-vc20** according to visual studio version
    set_property(TARGET glfw PROPERTY
                 IMPORTED_LOCATION "${GLFW_WIN_DIR}/lib-vc2017/glfw3.dll")
    set_property(TARGET glfw PROPERTY
                 IMPORTED_IMPLIB "${GLFW_WIN_DIR}/lib-vc2017/glfw3dll.lib")
    target_include_directories(glfw INTERFACE "${GLFW_WIN_DIR}/include")

    target_link_libraries(OpenGLDemos PUBLIC glfw)

    # TODO: also link against GLEW
else()
    find_package(glfw3 REQUIRED)
    # find_package(glfw3 3.3 REQUIRED)
    target_link_libraries(OpenGLDemos PUBLIC glfw)

    find_package(GLEW REQUIRED)
    target_link_libraries(OpenGLDemos PUBLIC GLEW::GLEW)
endif()

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



