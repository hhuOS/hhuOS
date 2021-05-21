#ifndef HHUOS_HEAPMEMORYMANAGER_H
#define HHUOS_HEAPMEMORYMANAGER_H

#include "lib/util/reflection/Prototype.h"
#include "MemoryManager.h"

namespace Kernel {

class HeapMemoryManager : public Util::Reflection::Prototype, public MemoryManager {

public:
    /**
     * Constructor.
     */
    HeapMemoryManager() = default;

    /**
     * Copy constructor.
     */
    HeapMemoryManager(const HeapMemoryManager &copy) = delete;

    /**
     * Assignment operator.
     */
    HeapMemoryManager &operator=(const HeapMemoryManager &other) = delete;

    /**
     * Destructor.
     */
    ~HeapMemoryManager() override = default;

    /**
     * Initialize the memory manager.
     *
     * @param startAddress Start address of the memory area to manage
     * @param endAddress End address of the memory area to manage
     */
    virtual void initialize(uint32_t startAddress, uint32_t endAddress);

    /**
	 * Allocate an aligned chunk of memory of a given size.
     *
     * This type of allocation may not be supported by every memory manager.
     * For example, it does not make sense to request allocated memory from a bitmap-based manager,
     * as such a manager always returns chunks with the same alignment,
	 *
	 * @param size Amount of memory to allocate
	 * @param alignment Alignment of the allocated chunk
     *
	 * @return Pointer to the allocated chunk of memory or nullptr if no chunk with the required size is available
	 */
    virtual void *allignedAlloc(uint32_t size, uint32_t alignment);

    /**
     * Reallocate a block of memory of a given size.
     *
     * If a new chunk needs to be allocated for the reallocation, the content
     * of the old chunk is copied into the new one up to the lesser of the new and old sizes.
     *
     * Reallocation may not be supported by every memory manager.
     *
     * @param ptr Pointer to the chunk of memory to reallocate
     * @param size Amount of new memory to allocate
     *
     * @return Pointer to the reallocated chunk of memory or nullptr if no chunk with the required size is available
     */
    virtual void *realloc(void *ptr, uint32_t size);

    /**
     * Reallocate a block of memory of a given size. The reallocated block will be aligned to a given alignment.
     *
     * If a new chunk needs to be allocated for the reallocation, the content
     * of the old chunk is copied into the new one up to the lesser of the new and old sizes.
     *
     * @param ptr Pointer to the chunk of memory to reallocate
     * @param size Amount of new memory to allocate
	 * @param alignment Alignment of the allocated chunk
     *
     * @return Pointer to the reallocated chunk of memory or nullptr if no chunk with the required size is available
     */
    virtual void *realloc(void *ptr, uint32_t size, uint32_t alignment);

    /**
	 * Free an allocated block of memory, that has been allocated with an alignment.
	 *
     * @param ptr Pointer to chunk of memory memory to be freed
	 * @param alignment Alignment of the chunk
	 */
    virtual void alignedFree(void *ptr, uint32_t alignment);
};

}

#endif
