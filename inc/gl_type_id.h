#ifndef GL_TYPE_ID_H
#define GL_TYPE_ID_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <optional>

template<typename T>
constexpr std::optional<GLenum> gl_type_to_id = {};

//template <>
//constexpr std::optional<GLenum> gl_type_id<GLhalf> = GL_HALF_FLOAT;
// -> problem: on CPU side half precision float is not availible
//             so GLhalf is 16-bit unsigned integer on CPU
//              -> cannot be distinguished from GLushort

template <>
constexpr std::optional<GLenum> gl_type_to_id<GLfloat> = GL_FLOAT;

template <>
constexpr std::optional<GLenum> gl_type_to_id<GLdouble> = GL_DOUBLE;

//template <>
//constexpr std::optional<GLenum> gl_type_id<GLfixed> = GL_FIXED;
// -> cannot be distinguished from GLint

template <>
constexpr std::optional<GLenum> gl_type_to_id<GLbyte> = GL_BYTE;

template <>
constexpr std::optional<GLenum> gl_type_to_id<GLubyte> = GL_UNSIGNED_BYTE;

template <>
constexpr std::optional<GLenum> gl_type_to_id<GLshort> = GL_SHORT;

template <>
constexpr std::optional<GLenum> gl_type_to_id<GLushort> = GL_UNSIGNED_SHORT;

template <>
constexpr std::optional<GLenum> gl_type_to_id<GLint> = GL_INT;

template <>
constexpr std::optional<GLenum> gl_type_to_id<GLuint> = GL_UNSIGNED_INT;

//template <>
//constexpr std::optional<GLenum> gl_type_to_id<GLuint> = GL_INT_2_10_10_10_REV;
// -> cannot be distinguished from GL_UNSIGNED_INT

//template <>
//constexpr std::optional<GLenum> gl_type_to_id<GLuint> = GL_UNSIGNED_INT_2_10_10_10_REV;
// -> cannot be distinguished from GL_UNSIGNED_INT

//template <>
//constexpr std::optional<GLenum> gl_type_to_id<GLuint> = GL_UNSIGNED_INT_10F_11F_11F_REV;
// -> cannot be distinguished from GL_UNSIGNED_INT

#endif // GL_TYPE_ID_H
