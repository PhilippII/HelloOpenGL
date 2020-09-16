#ifndef VERTEXBUFFERLAYOUT_H
#define VERTEXBUFFERLAYOUT_H

#include <GL/glew.h>

#include <vector>
#include <string>

#include "debug_utils.h"
#include "GLShaderProgram.h" // to query attribute locations
#include "gl_type_id.h"

enum class VariableType {
    // type of the actual shader variable
    // which may be different from the type of the data in the buffer
    // (e.g. we might cast integers in the buffer to float variables in the shader)
    FLOAT, NORMALIZED_FLOAT, DOUBLE, INT
};


struct VertexAttributeLayout {
    // glBindVertexBuffer   (GLintptr)
    // glVertexAttribFormat (GLuint)
    // glVertexAttribPointer(const GLvoid*)
    GLuint offset;
    GLint dimCount;
    GLenum componentType;
    VariableType castTo;

    // glVertexAttribFormat (GLuint)
    // glVertexAttribPointer(GLuint)
    GLuint location;
    std::string name;
};

class VertexBufferLayout
{
public:
    using count_type = std::vector<VertexAttributeLayout>::size_type;

    using offset_type = GLuint; // see VertexAttributeLayout::offset


    // glBindVertexBuffer       (GLintptr)
    // glVertexArrayVertexBuffer(GLsizei)
    // glVertexAttribPointer    (GLsizei)
    using stride_type = GLsizei;

    using loc_type = GLuint; // see VertexAttributeLayout::location

    enum class TypeCategory {
        FLOAT_SINGLE_PREC,  // glVertexAttribPointer( normalized = GL_FALSE )
        FLOAT_DOUBLE_PREC,  // glVertexAttribLPointer( )
                            // glVertexAttribPointer( normalized = GL_FALSE ) (not recommended but possible)
        INT_NOT_PACKED,     // glVertexAttribIPointer( )
                            // glVertexAttribPointer( normalized = GL_FALSE or GL_TRUE )
        INT_PACKED          // glVertexAttribPointer( normalized = GL_FALSE or GL_TRUE )
                            //              ( if dimCount = GL_BGRA must be normalized = GL_TRUE )
    };

    VertexBufferLayout();

    void append(GLint dimCount, GLenum componentType, VariableType castTo, loc_type location, std::string name = std::string());

    void append(GLint dimCount, GLenum componentType, VariableType castTo, std::string name = std::string())
    {
        append(dimCount, componentType, castTo,
               static_cast<loc_type>(m_attributes.size()), name);
    }

    void append(GLint dimCount, GLenum componentType, std::string name = std::string());

    template<typename T>
    void append(GLint dimCount, VariableType castTo, loc_type location, std::string name = std::string()) {
        constexpr auto gl_type = gl_type_to_id<T>;
        static_assert(gl_type);
        if constexpr(gl_type->dimCount != 1) {
            myAssert(dimCount == 1);
            // NOTE: for packed types (like GL_INT_2_10_10_10_REV​)
            //      I actually expect
            //      myAssert(dimCount == gl_type->dimCount)
            //      however packed types cannot be used with the templated
            //      overload of append() anyway as they cannot be distinguished from
            //      GL_UNSIGNED_INT
            append(gl_type->dimCount, gl_type->id, castTo, location, name);
        } else {
            append(         dimCount, gl_type->id, castTo, location, name);
        }
    }

    template<typename T>
    void append(GLint dimCount, VariableType castTo, std::string name = std::string()) {
        constexpr auto gl_type = gl_type_to_id<T>;
        static_assert(gl_type);
        if constexpr(gl_type->dimCount != 1) {
            myAssert(dimCount == 1); // note comment on above overload
            append(gl_type->dimCount, gl_type->id, castTo, name);
        } else {
            append(         dimCount, gl_type->id, castTo, name);
        }
    }

    template<typename T>
    void append(GLint dimCount, std::string name = std::string()) {
        constexpr auto gl_type = gl_type_to_id<T>;
        static_assert(gl_type);
        if constexpr(gl_type->dimCount != 1) {
            myAssert(dimCount == 1); // note comment on above overload
            append(gl_type->dimCount, gl_type->id, name);
        } else {
            append(         dimCount, gl_type->id, name);
        }
    }

    VertexBufferLayout& operator+=(const VertexBufferLayout& other);

    void setDefaultLocations();

    const std::vector<VertexAttributeLayout>& getAttributes() const {
        return m_attributes;
    }

    stride_type getStride() const {
        return m_stride;
    }

    void setLocations(const GLShaderProgram& program);

    static TypeCategory getTypeCategory(GLenum componentType);

    //static bool isFloat(GLenum componentType) {
    //    TypeCategory category = getTypeCategory(componentType);
    //    return category == TypeCategory::FLOAT_SINGLE_PREC || category == TypeCategory::FLOAT_DOUBLE_PREC;
    //}

    //static bool isInteger(GLenum componentType) {
    //    TypeCategory category = getTypeCategory(componentType);
    //    return category == TypeCategory::INT_PACKED || category == TypeCategory::INT_NOT_PACKED;
    //}
    static bool isValidCast(GLenum componentType, VariableType castTo);
private:
    std::vector<VertexAttributeLayout> m_attributes;
    stride_type m_stride;

    static GLuint getAttributeSize(GLint dimCount, GLenum componentType);
    static VariableType getDefaultCast(GLenum componentType);
    static bool isValidDimension(GLint dimCount, GLenum componentType, VariableType castTo);
};

VertexBufferLayout operator+(VertexBufferLayout a, const VertexBufferLayout& b);



#endif // VERTEXBUFFERLAYOUT_H
