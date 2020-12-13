cmake_minimum_required(VERSION 3.13)
# CMP0072 -policy regarding find_package(OpenGL ...)
# is new in version 3.11
# VS_DEBUGGER_ENVIRONMENT requires version 3.13

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
    src/GLFramebufferObject.cxx
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
    src/demos/DemoFilter.cxx
    src/demos/DemoFramebuffer.cxx

    # hopefully cmake understands by the file suffix that the
    # following files should only be shown in qt-creator's outliner
    # but not compiled:
    res/shaders/BlendVertColUniCol.shader
    res/shaders/ShadelessTexture.shader
    res/shaders/PhongReflModel.shader
    res/shaders/TexturedPhongRefl.shader
    res/shaders/Filter.shader
)

target_include_directories(OpenGLDemos PUBLIC
                           inc
                           inc/demos
                           )

target_link_libraries(OpenGLDemos 
                      PUBLIC warning_flags
                      PUBLIC debug_stl)

if(WIN32)
    # select glfw version corresponding to the newest visual studio version, 
    # which is not newer then the actually used visual studio version defined 
    # by MSVC_TOOLSET_VERSION.
    # if MSVC_TOOLSET_VERSION is undefined use newest version. (=2019)
    # If all available versions are newer than MSVC_TOOLSET_VERSION 
    # use oldest version. (=2012)
    if(MSVC_TOOLSET_VERSION GREATER_EQUAL 142 OR NOT MSVC_TOOLSET_VERSION)
        set(GLFW_MSVC_YEAR 2019)
    elseif(MSVC_TOOLSET_VERSION GREATER_EQUAL 141)
        set(GLFW_MSVC_YEAR 2017)
    elseif(MSVC_TOOLSET_VERSION GREATER_EQUAL 140)
        set(GLFW_MSVC_YEAR 2015)
    elseif(MSVC_TOOLSET_VERSION GREATER_EQUAL 120)
        set(GLFW_MSVC_YEAR 2013)
    else()
        set(GLFW_MSVC_YEAR 2012)
    endif()
    message(STATUS "GLFW_MSVC_YEAR=${GLFW_MSVC_YEAR}")

    # glfw:
    add_library(glfw SHARED IMPORTED)
    set(GLFW_WIN_DIR "${PROJECT_SOURCE_DIR}/3rd_party/windows/glfw-3.3.2.bin.WIN64")
    # TODO: select correct folder lib-vc20** according to visual studio version
    set_property(TARGET glfw PROPERTY
                 IMPORTED_LOCATION "${GLFW_WIN_DIR}/lib-vc${GLFW_MSVC_YEAR}/glfw3.dll")
    set_property(TARGET glfw PROPERTY
                 IMPORTED_IMPLIB "${GLFW_WIN_DIR}/lib-vc${GLFW_MSVC_YEAR}/glfw3dll.lib")
    target_include_directories(glfw INTERFACE "${GLFW_WIN_DIR}/include")
    target_compile_definitions(glfw INTERFACE "GLFW_DLL")

    # glew:
    add_library(glew STATIC IMPORTED)
    set(GLEW_WIN_DIR "${PROJECT_SOURCE_DIR}/3rd_party/windows/glew-2.1.0")
    set_property(TARGET glew PROPERTY
                 IMPORTED_LOCATION "${GLEW_WIN_DIR}/lib/Release/x64/glew32s.lib")
    target_include_directories(glew INTERFACE "${GLEW_WIN_DIR}/include")
    target_compile_definitions(glew INTERFACE "GLEW_STATIC")

    # add aliases so we can use the same names
    # whether we imported the library manually 
    # or via find_package():
    add_library(glfw_alias ALIAS glfw)
    add_library(glew_alias ALIAS glew)
else()
    find_package(glfw3 REQUIRED)
    # find_package(glfw3 3.3 REQUIRED)
    find_package(GLEW REQUIRED)
    
    # add aliases so we can use the same names
    # whether we imported the library manually 
    # or via find_package():
    add_library(glfw_alias ALIAS glfw)
    add_library(glew_alias ALIAS GLEW::GLEW)
endif()
target_link_libraries(OpenGLDemos 
                      PUBLIC glfw_alias
                      PUBLIC glew_alias)

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


if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    #file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res SYMBOLIC)
    set(RESOURCE_FILES  "shaders/BlendVertColUniCol.shader"
                        "shaders/ShadelessTexture.shader"
                        "shaders/PhongReflModel.shader"
                        "shaders/TexturedPhongRefl.shader"
                        "shaders/Filter.shader"
                        "textures/alpha_texture_test.png"
                        "textures/solid_test_texture.png"
                        "textures/uv_grid.png"
                        "meshes/suzanne_horn_smooth_subdiv_1.obj"
                        "meshes/3rd_party/3D_Model_Haven/GothicBed_01/GothicBed_01.obj"
                        "meshes/3rd_party/3D_Model_Haven/GothicBed_01/GothicBed_01_Textures/GothicBed_01_8-bit_Diffuse.png")
    add_custom_target(copy_resource_files)
    foreach(RES_FILE ${RESOURCE_FILES})
        add_custom_command(TARGET copy_resource_files
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                                ${CMAKE_CURRENT_SOURCE_DIR}/res/${RES_FILE} 
                                ${CMAKE_CURRENT_BINARY_DIR}/res/${RES_FILE} )
    endforeach()
    add_dependencies(OpenGLDemos copy_resource_files)
endif()

if(WIN32)
    # if a .dll is not found when running the OpenGLDemos executable, then add
    # the corresponding library target to this list:
    set(LIBS "glfw_alias")
    set(WINDOWS_QUASI_RPATH "")
    foreach(LIB ${LIBS})
        get_property(LIB_TYPE TARGET ${LIB} PROPERTY TYPE)
        if(${LIB_TYPE} STREQUAL "SHARED_LIBRARY")
            list(APPEND WINDOWS_QUASI_RPATH "$<TARGET_FILE_DIR:${LIB}>")
        endif()
    endforeach()
    message(STATUS "WINDOWS_QUASI_RPATH=${WINDOWS_QUASI_RPATH}")
    set_property(TARGET OpenGLDemos
                 PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=${WINDOWS_QUASI_RPATH};%PATH%")

    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT OpenGLDemos)
endif()
