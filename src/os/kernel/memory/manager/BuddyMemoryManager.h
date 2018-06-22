#ifndef __BUDDY_MANAGER_H__
#define __BUDDY_MANAGER_H__

#include <cstdint>
#include "kernel/memory/manager/MemoryManager.h"

struct buddyNode {
    void *addr;
    struct buddyNode *next;
};

class BuddyMemoryManager : MemoryManager {
private:
	uint8_t min_order = 0;
	uint8_t max_order = 0;
	struct buddyNode **freelist = nullptr;

public:
	BuddyMemoryManager(uint32_t memoryStartAddress, uint32_t memoryEndAddress, uint8_t min_order);
	~BuddyMemoryManager();
	void init();
	void cleanup();

	void* alloc(uint32_t size);
	void free(void* ptr);

	void dump();
};

#endif
