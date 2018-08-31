#ifndef __BUDDY_MANAGER_H__
#define __BUDDY_MANAGER_H__

#include <cstdint>
#include "kernel/memory/manager/MemoryManager.h"

struct buddyNode {
    void *addr;
    struct buddyNode *next;
};

class BuddyMemoryManager : public MemoryManager {
private:
	uint8_t min_order = 0;
	uint8_t max_order = 0;
	struct buddyNode **freelist = nullptr;

public:
	BuddyMemoryManager(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap, uint8_t minOrder = 4);

	~BuddyMemoryManager() override;

	void* alloc(uint32_t size) override;

	void free(void* ptr) override;

	void dump();
};

#endif
