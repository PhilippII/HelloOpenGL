#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#ifndef WIN32
#include <signal.h> // for SIGTRAP in myAssert
#endif

#ifdef NDEBUG
#define myAssert(x)
#else
#ifdef WIN32
#define myAssert(x) if (!(x)) __debugbreak()
#else
#define myAssert(x) if (!(x)) raise(SIGTRAP)
#endif
#endif

#ifdef NDEBUG
#define debugDo(x)
#else
#define debugDo(x) x
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
