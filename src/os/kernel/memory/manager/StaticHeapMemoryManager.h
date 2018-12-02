#ifndef HHUOS_STATICHEAPMEMORYMANAGER_H
#define HHUOS_STATICHEAPMEMORYMANAGER_H

#include "MemoryManager.h"

/**
 * Memory manager, that only uses a pointer to manage a memory area. Every allocation just returns the current position
 * of this pointer and lets it advance by the requested amount of bytes. This means, that allocated memory cannot be
 * freed. This manager's implementation of free() does not do anything.
 */
class StaticHeapMemoryManager : public MemoryManager {

private:

    uint32_t currentPosition;

public:

    /**
     * Constructor.
     *
     * @param memoryStartAddress Start address of the memory area to manage
     * @param memoryEndAddress End address of the memory area to manage
     */
    StaticHeapMemoryManager(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap = false);

    /**
     * Copy-constructor.
     */
    StaticHeapMemoryManager(const StaticHeapMemoryManager &copy) = delete;

    /**
     * Destructor.
     */
    ~StaticHeapMemoryManager() override = default;

    /**
     * Overriding function from MemoryManager.
     */
    void* alloc(uint32_t size) override;

    /**
     * Overriding function from MemoryManager.
     */
    void* alloc(uint32_t size, uint32_t alignment) override;

    /**
     * Overriding function from MemoryManager.
     */
    void *realloc(void *ptr, uint32_t size) override;

    /**
     * Overriding function from MemoryManager.
     */
    void *realloc(void *ptr, uint32_t size, uint32_t alignment) override;

    /**
     * Overriding function from MemoryManager.
     */
    void free(void* ptr) override;

    /**
     * Overriding function from MemoryManager.
     */
    void free(void* ptr, uint32_t alignment) override;

    /**
     * Dump the current position.
     */
    void dump() override;

};

#endif
