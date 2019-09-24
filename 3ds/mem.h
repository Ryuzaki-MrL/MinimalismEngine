#ifndef MEM_H
#define MEM_H

#include <3ds.h>

/* Platform-specific allocators */

inline void* memalloc(size_t size) { return linearAlloc(size); }
inline void memfree(void* mem) { linearFree(mem); }

#endif /* MEM_H */
