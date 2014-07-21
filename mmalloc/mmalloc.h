#ifndef _MMALLOC_H_
#define _MMALLOC_H_

void* mmalloc(size_t size);
void mfree(void* ptr);
void *mcalloc(unsigned int nb_elem, size_t size);
void * mrealloc(void* ptr, size_t size);

#define malloc(size) mmalloc(size)
#define free(ptr) mfree(ptr)
#define calloc(nb_elem,size) mcalloc(nb_elem, size)
#define realloc(ptr,size) mrealloc(ptr, size)

#endif
