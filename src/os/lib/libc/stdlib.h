/*****************************************************************************
 *                                                                           *
 *                               S T D L I B                                 *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Description:     Provides several general utility macros functions.       *
 *                                                                           *
 *                                                                           *
 * Author:          Fabian Ruhland, HHU, 16.10.2017                          *
 *****************************************************************************/

#ifndef __stdlib_include__
#define __stdlib_include__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO:
//  This is already defined in cstddef and stddef
// #define NULL            0
#define EXIT_FAILURE    1
#define EXIT_SUCCESS    0

typedef unsigned int size_t;
//typedef long            wchar_t;

extern void *malloc(size_t);

extern void *aligned_alloc(uint32_t alignment, size_t size);

extern void free(void *);

extern void *realloc(void *, size_t);

#ifdef __cplusplus
}
#endif

#endif
