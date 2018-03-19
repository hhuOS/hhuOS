//
// Created by burak on 02.03.18.
//

#ifndef __BITMAPMEMORYMANAGER_H__
#define __BITMAPMEMORYMANAGER_H__

#include "MemoryManager.h"
#include <lib/String.h>

#define DEBUG_BMM 0

class BitmapMemoryManager : public MemoryManager{

protected:
    String name;
    // bitmap-array for free page frames
    uint32_t *freeBitMap;
    // length of bitmap-array
    uint16_t freeBitMapLength;
    uint16_t bmpIndex;
    // shall allocated memory be zeroed? -> needed if memory is allocated for PageTables
    bool zeroMemory = false;

public:
    BitmapMemoryManager(uint32_t startAddress, uint32_t endAddress, String name, bool zeroMemory);

    virtual void init() = 0;

    uint32_t alloc();
    // free memory of a page
    uint32_t free(uint32_t address);
    // bitmap dump for debugging purposes
    void dump(){}
};


#endif
