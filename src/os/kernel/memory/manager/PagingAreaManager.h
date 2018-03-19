/* Paging Area Manager - manages the part of virtual memory resverved for 
 * page tables and directories. A bitmap-based implementation.
 * 
 * Burak Akguel, Christian Gesse, HHU 2017 */

#ifndef __PAGINGAREAMANAGER_H__
#define __PAGINGAREAMANAGER_H__

#include <stdint.h>
#include "kernel/memory/manager/BitmapMemoryManager.h"

class PagingAreaManager : public BitmapMemoryManager {

public:
    PagingAreaManager();

// init the Paging Area Manager -> instead of constructor
    void init();

};

#endif