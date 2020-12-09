#include "DemoFilter.h"

#include <array>
#include <filesystem>

#include "VertexBufferLayout.h"

namespace demo {

DemoFilter::DemoFilter(GLRenderer &renderer)
    : Demo(renderer)
{
    namespace fs = std::filesystem;
    // init shader:
    m_shaderP = std::make_unique<GLShaderProgram>(fs::path("res/shaders/Filter.shader",
                                                           fs::path::format::generic_format));


    // init screen filling quad (VertexBuffer, IndexBuffer, VertexArray):
    //  3--2
    //  | /|
    //  |/ |
    //  0--1
    std::array<GLfloat, 4 * 2> positions {
        -1.0, -1.0,
         1.0, -1.0,
         1.0,  1.0,
        -1.0,  1.0
    };

    std::array<GLuint, 2 * 3> indices {
        0, 1, 2,
        0, 2, 3
    };

    m_vbo = std::make_unique<GLVertexBuffer>(positions.size() * sizeof(GLfloat), positions.data());

    m_ibo = std::make_unique<GLIndexBuffer>(GL_UNSIGNED_INT, indices.size(), indices.data());

    m_vao = std::make_unique<GLVertexArray>();
    VertexBufferLayout layout;
    layout.append<float>(2, "ndc_pos");
    layout.setLocations(*m_shaderP);
    m_vao->addBuffer(*m_vbo, layout);


    // init texture:
    m_texture = std::make_unique<GLTexture>(fs::path("res/textures/solid_test_texture.png",
                                                     fs::path::format::generic_format),
                                            3, false,
                                            texture_sampling_presets::noFilter);
        // while technically the image is sRGB we do not explicitly use an sRGB texture here.
        // As the framebuffer is sRGB too we do not need to do any conversion from/to sRGB anyway.
    m_texture->bind(texUnit);
    m_shaderP->setUniform1i("tex", texUnit);
}

void DemoFilter::OnRender()
{
    getRenderer().draw(*m_vao, *m_ibo, *m_shaderP);
}

}
