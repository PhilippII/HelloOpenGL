#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <signal.h> // for SIGTRAP in myAssert

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


#endif // RENDERER_H
