#ifndef MEM_H
#define MEM_H

/* Platform-specific allocators */

inline void* memalloc(size_t size) { return malloc(size); }
inline void memfree(void* mem) { free(mem); }

#endif /* MEM_H */
