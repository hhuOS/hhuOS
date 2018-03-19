/*****************************************************************************
 *                                                                           *
 *                      S Y S T E M _ I N T E R F A C E                      *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Description:     Interface between the C-library and the operating        *
 *                  system.                                                  *
 *                                                                           *
 *                                                                           *
 * Author:          Fabian Ruhland, HHU, 16.10.2017                          *
 *****************************************************************************/

#ifndef __system_interface_include__
#define __system_interface_include__

#include "lib/libc/stdlib.h"

#ifdef __cplusplus
extern "C" {
    #include "kernel/memory/manager/HeapMemoryManager.h"
#endif

void *sys_alloc_mem(size_t size, uint32_t alignment);
void sys_free_mem(void *);

#ifdef __cplusplus
}
#endif 

#endif
