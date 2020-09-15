#ifndef GL_TYPE_ID_H
#define GL_TYPE_ID_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <optional>
#include <array>

struct VertexAttribType {
    GLenum id;
    GLint dimCount;
};

template<typename T>
constexpr std::optional<VertexAttribType> gl_type_to_id = {};

//template <>
//constexpr std::optional<GLenum> gl_type_id<GLhalf> = GL_HALF_FLOAT;
// -> problem: on CPU side half precision float is not availible
//             so GLhalf is 16-bit unsigned integer on CPU
//              -> cannot be distinguished from GLushort

template <>
constexpr std::optional<VertexAttribType> gl_type_to_id<GLfloat> = VertexAttribType{GL_FLOAT, 1};

template <>
constexpr std::optional<VertexAttribType> gl_type_to_id<GLdouble> = VertexAttribType{GL_DOUBLE, 1};

//template <>
//constexpr std::optional<GLenum> gl_type_id<GLfixed> = GL_FIXED;
// -> cannot be distinguished from GLint

template <>
constexpr std::optional<VertexAttribType> gl_type_to_id<GLbyte> = VertexAttribType{GL_BYTE, 1};

template <>
constexpr std::optional<VertexAttribType> gl_type_to_id<GLubyte> = VertexAttribType{GL_UNSIGNED_BYTE, 1};

template <>
constexpr std::optional<VertexAttribType> gl_type_to_id<GLshort> = VertexAttribType{GL_SHORT, 1};

template <>
constexpr std::optional<VertexAttribType> gl_type_to_id<GLushort> = VertexAttribType{GL_UNSIGNED_SHORT, 1};

template <>
constexpr std::optional<VertexAttribType> gl_type_to_id<GLint> = VertexAttribType{GL_INT, 1};

template <>
constexpr std::optional<VertexAttribType> gl_type_to_id<GLuint> = VertexAttribType{GL_UNSIGNED_INT, 1};

//template <>
//constexpr std::optional<GLenum> gl_type_to_id<GLuint> = GL_INT_2_10_10_10_REV;
// -> cannot be distinguished from GL_UNSIGNED_INT

//template <>
//constexpr std::optional<GLenum> gl_type_to_id<GLuint> = GL_UNSIGNED_INT_2_10_10_10_REV;
// -> cannot be distinguished from GL_UNSIGNED_INT

//template <>
//constexpr std::optional<GLenum> gl_type_to_id<GLuint> = GL_UNSIGNED_INT_10F_11F_11F_REV;
// -> cannot be distinguished from GL_UNSIGNED_INT


template<typename C, std::size_t N>
constexpr std::optional<VertexAttribType> gl_type_to_id<std::array<C, N>> = VertexAttribType{gl_type_to_id<C>->id, N};

#endif // GL_TYPE_ID_H
