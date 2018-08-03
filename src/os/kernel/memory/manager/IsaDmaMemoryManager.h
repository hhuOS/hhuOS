#ifndef HHUOS_ISADMAMEMORYMANAGER_H
#define HHUOS_ISADMAMEMORYMANAGER_H

#include "BitmapMemoryManager.h"

class IsaDmaMemoryManager : public BitmapMemoryManager {

    public:

	/**
	 * Constructor
	 */
    IsaDmaMemoryManager();

    static const constexpr uint32_t ISA_DMA_START_ADDRESS = 0x00008000;
    static const constexpr uint32_t ISA_DMA_END_ADDRESS = ISA_DMA_START_ADDRESS + 0x88000;
    static const constexpr uint32_t ISA_DMA_BUF_SIZE = 0x10000;
};

#endif
