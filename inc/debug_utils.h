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


#endif // DEBUG_UTILS_H
