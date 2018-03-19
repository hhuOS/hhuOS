/* Memory manager for the kernel heap
 * 
 * Burak Akguel, Christian Gesse, HHU 2017 */

#ifndef __HEAPMEMORYMANAGER_H__
#define __HEAPMEMORYMANAGER_H__

#include <stdint.h>
#include "kernel/Spinlock.h"
#include "MemoryManager.h"


struct Chunk{
	Chunk* prev;		// voriger Chunk
	Chunk* next;		// nächster Chunk
	bool allocated;		// Chunk benutzt oder nicht?
	uint32_t size;	// Speicherplatz im Chunk
};

class HeapMemoryManager : public MemoryManager {

private:
    // Lock for Memory Management
    Spinlock* lock = nullptr;

    Chunk* firstChunk;

public:
    HeapMemoryManager(uint32_t startAddress, uint32_t endAddress);
    ~HeapMemoryManager();
    void init();
    void* alloc(uint32_t size);
    void free(void* ptr);

    void dump();
};

#endif
