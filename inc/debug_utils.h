#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

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


#endif // DEBUG_UTILS_H
