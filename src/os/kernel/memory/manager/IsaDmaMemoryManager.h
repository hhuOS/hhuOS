#ifndef HHUOS_ISADMAMEMORYMANAGER_H
#define HHUOS_ISADMAMEMORYMANAGER_H

#include "BitmapMemoryManager.h"

/**
 * Manages the memory from 0x8000 to 0x88000. hhuOS uses this area for DMA transfers via the ISA bus.
 * This manager allocates blocks with a size of 64KB, as this is the maximum size,
 * that is supported by the ISA controller.
 */
class IsaDmaMemoryManager : public BitmapMemoryManager {

public:
	/**
	 * Constructor
	 */
    IsaDmaMemoryManager() noexcept;

    /**
     * Allocate a 64KB buffer, which can be used for DMA transfers via the ISA bus.
     */
    void *allocateDmaBuffer();

    static const constexpr uint32_t ISA_DMA_BUF_SIZE = 0x10000;
};

#endif
