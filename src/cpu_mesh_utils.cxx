#include "cpu_mesh_utils.h"

// used by addIndexBuffer(...)
bool areBytesEqual(const GLbyte *a, const GLbyte *b, unsigned int size)
{
    unsigned int i;
    for (i = 0; i < size && a[i] == b[i]; ++i) {}
    return i == size;
}

