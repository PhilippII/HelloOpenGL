#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#ifndef WIN32
#include <signal.h> // for SIGTRAP in myAssert
#endif

#ifdef NDEBUG
#define ASSERT(x)
#else
#ifdef WIN32
#define ASSERT(x) if (!(x)) __debugbreak()
#else
#define ASSERT(x) if (!(x)) raise(SIGTRAP)
#endif
#endif

#ifdef NDEBUG
#define DEBUG_DO(x)
#else
#define DEBUG_DO(x) x
#endif


#endif // DEBUG_UTILS_H
