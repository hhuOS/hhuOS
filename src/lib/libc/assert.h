#ifndef LIBC_ASSERT
#define LIBC_ASSERT

#include <stdio.h>

#ifdef NDEBUG

#define assert(condition) ((void) 0)

#else 
	
#define assert(condition) if ((condition) == 0) fprintf(stderr, "%s:%d Assert failed in function %s\n", __FILE__, __LINE__, __func__)

#endif


#endif