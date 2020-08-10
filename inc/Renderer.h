#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <signal.h> // for SIGTRAP in myAssert

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#ifdef NDEBUG
#define myAssert(x)
#else
#define myAssert(x) if (!(x)) raise(SIGTRAP)
#endif

#ifdef NDEBUG
#define GLCall(x) x
#else
#define GLCall(x) GLClearError();\
    x;\
    myAssert(GLLogCall(#x, __FILE__, __LINE__))
#endif

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};

#endif // RENDERER_H
