/*
 * MemoryManager.h
 *
 *  Created on: 02.03.2018
 *      Author: christian
 */

#ifndef __KERNEL_MEMORY_MEMORYMANAGER_H__
#define __KERNEL_MEMORY_MEMORYMANAGER_H__

#include <cstdint>

class MemoryManager {

protected:
	uint32_t startAddress;
	uint32_t endAddress;
	uint32_t freeMemory = 0;
	bool initialized = false;

public:
	MemoryManager(uint32_t startAddress, uint32_t endAddress) {
		this->startAddress = startAddress;
		this->endAddress = endAddress;
	}

	virtual void init() = 0;

	bool isInitialized() {
		return initialized;
	}

	uint32_t getEndAddress() {
		return endAddress;
	}

	uint32_t getFreeMemory() {
		return freeMemory;
	}

	uint32_t getStartAddress() {
		return startAddress;
	}
};


#endif /* SRC_OS_KERNEL_MEMORY_MEMORYMANAGER_H_ */
