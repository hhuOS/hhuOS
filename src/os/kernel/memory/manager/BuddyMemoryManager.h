/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

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
