/* List-based memory manager for the kernel heap
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */

#include <lib/libc/printf.h>
#include "HeapMemoryManager.h"

#include "../SystemManagement.h"
#include "kernel/memory/MemLayout.h"

/**
 * Constructor -calls constructor of base class.

 */
HeapMemoryManager::HeapMemoryManager(uint32_t memoryStartAddress, uint32_t memoryEndAddress) : MemoryManager(memoryStartAddress, memoryEndAddress) {
}

/**
 * Destructor.
 */
HeapMemoryManager::~HeapMemoryManager() {
	if(lock){
		delete lock;
	}
}

/**
 * Initializes this manager. Sets up first list-entry.
 */
void HeapMemoryManager::init() {
    freeMemory = memoryEndAddress - memoryStartAddress;

    if(freeMemory < sizeof(Chunk) + (sizeof(Chunk) + sizeof(Spinlock))) {
        // Available Kernel-Memory is too small for a Chunk
		firstChunk = 0;
	}else {
		// set up first Chunk of memory
		firstChunk = (Chunk*)memoryStartAddress;
		firstChunk->next = 0;
		firstChunk->prev = 0;
		firstChunk->allocated = false;
		freeMemory -= sizeof(Chunk);
		firstChunk->size = freeMemory;
	}
	
    // allocate a Spinlock to lock the memory list
	lock = new Spinlock();

    initialized = true;
}

/**
 * Allocate memory block with given size.
 */
void* HeapMemoryManager::alloc(uint32_t size) {

	// size = 0 is not allowed
	if(size < 1) {
		return 0;
	}

	// during init a lock must be allocated -> in this allocation we cannto use the lock
	if(initialized) {
		lock->acquire();
	}

	// Chunk of memory that will be returned
	Chunk* returnChunk = 0;
	Chunk* lastVisited = firstChunk;
	
	// search for a Chunk - first fit model
	do{
		// check if this chunk is free and has enough memory
		if(lastVisited->size >= size && !lastVisited->allocated) {
			// we have found a chunk of memory
			returnChunk = lastVisited;
			break;
		}

		// look for next chunk
		lastVisited = lastVisited->next;
	// loop thorugh memory list until we reach the end (next-pointer = 0)
	} while(lastVisited != 0);

	// found no free chunk with enough memory
	if(returnChunk == 0) {
		if(initialized) {
			lock->release();
		}
		// return nullpointer
#if THROW_EXCEPTION
		Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
#endif
		return nullptr;
	}
	
	// the chunk has more memory than needed -> split into 2 chunks
	if(returnChunk-> size >= sizeof(Chunk) + size + 1) {
		// get "next" Chunk and skip the memory we want to allocate
		Chunk* tmp = (Chunk*)((unsigned int)returnChunk + sizeof(Chunk) + size);
		// set next and prev pointer of new chunk
		tmp->prev = returnChunk;
		tmp->next = returnChunk->next;
		// set previous pointer of succeeding chunk
		if(returnChunk->next) {
			returnChunk->next->prev = tmp;
		}
		// set next pointer of previous chunk (that was allocated)
		returnChunk->next = tmp;
		// the new chunk is not allocated
		tmp->allocated = false;
		freeMemory -= sizeof(Chunk);
		// set size of the new created chunk
		tmp->size = returnChunk->size - size - sizeof(Chunk);
		// update size of allocated chunk
		returnChunk->size = size;
	}
	// the chunk is now allocated
	returnChunk->allocated = true;
	// update free memory
	freeMemory -= returnChunk->size;
	// release lock
	if(initialized) {
		lock->release();
	}
	// return pointer to the usable memory skipping the header
	return (void*)((unsigned int)returnChunk + sizeof(Chunk));
}

/**
 * Frees a given memory block
 */
void HeapMemoryManager::free(void* ptr) {
	// Prevent null pointer access
	if (ptr == nullptr) {
		return;
	}

	// block if we try to free outside memory area
	if((uint32_t) ptr < memoryStartAddress || (uint32_t) ptr >= memoryEndAddress){
		return;
	}

	// calculate ptr to chunk with metadata/list header
	Chunk* chunk = (Chunk*)((unsigned int)ptr - sizeof(Chunk));

	lock->acquire();

	// Chunk is no longer reserved/allocated
	chunk->allocated = false;
	// update free memory
	freeMemory += chunk->size;

	//if preceding chunks are also free -> merge them
	while(chunk->prev) {
		// we want to merge only with free chunks
		if(chunk->prev->allocated) {
			break;
		}
		// one chunkheader is not needed anymore
		freeMemory += sizeof(Chunk);
		// calculate size of new chunk
		chunk->prev->size += chunk->size + sizeof(Chunk);
		// update next pointer
		chunk->prev->next = chunk->next;
		// change previous pointer of next chunk
		if(chunk->next) 
			chunk->next->prev = chunk->prev;

		// get next previous chunk
		chunk = chunk->prev;
	}
	
	//if succeeding chunks are also free -> merge them
	while(chunk->next) {
		// check if succeeding block is free
		if(chunk->next->allocated) {
			break;
		}
		// one chunkheader is not needed anymore
		freeMemory += sizeof(Chunk);
		// update chunksize
		chunk->size += chunk->next->size + sizeof(Chunk);
		// update nextpointer
		chunk->next = chunk->next->next;
		
		// if next chunk exists, update previous pointer of it
		if(chunk->next) 
			chunk->next->prev = chunk;
	
	}

	// if the free chunk has more than 4kb of memory, a page can possibly be unmapped
	if(chunk->size >= 4*1024 && SystemManagement::getInstance()->isInitialized()) {
		uint32_t addr = (uint32_t) chunk;
		uint32_t chunkEndAddr = addr + (sizeof(Chunk) + chunk->size);

		// try to unmap the free memory, not the list header!
		SystemManagement::getInstance()->unmap(addr + sizeof(Chunk), chunkEndAddr - 1);
	}

	// release lock
	lock->release()	;


}

/**
 * Dump memory list
 */
void HeapMemoryManager::dump() {
    printf("  Memory dump\n");
    printf("  ===========\n");

    Chunk* tmp = firstChunk;

    while ( tmp != 0 ) {
		printf("    Start: %x" , (Chunk*)((uint8_t*)tmp + sizeof(Chunk)));
		printf(", End: %x" , (Chunk*)((uint8_t*)tmp + sizeof(Chunk) + tmp->size));
		printf(", Allocated: %d" , tmp->allocated);
		printf("  Next: %x", tmp->next);
		printf(", Size: %d\n" , tmp->size);

        tmp = tmp->next;
    }

    printf("\n");
}
