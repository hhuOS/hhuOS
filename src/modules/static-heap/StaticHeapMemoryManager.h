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

#ifndef HHUOS_STATICHEAPMEMORYMANAGER_H
#define HHUOS_STATICHEAPMEMORYMANAGER_H

#include "kernel/memory/manager/MemoryManager.h"

/**
 * Memory manager, that only uses a pointer to manage a memory area. Every allocation just returns the current position
 * of this pointer and lets it advance by the requested amount of bytes. This means, that allocated memory cannot be
 * freed. This manager's implementation of free() does not do anything.
 */
class StaticHeapMemoryManager : public MemoryManager {

private:

    uint32_t currentPosition;

    Spinlock lock;

    static const constexpr char *NAME = "StaticHeapMemoryManager";

public:

    PROTOTYPE_IMPLEMENT_CLONE(StaticHeapMemoryManager);

    /**
     * Constructor.
     */
    StaticHeapMemoryManager();

    /**
     * Copy-constructor.
     */
    StaticHeapMemoryManager(const StaticHeapMemoryManager &copy);

    /**
     * Destructor.
     */
    ~StaticHeapMemoryManager() override = default;

    /**
     * Overriding function from MemoryManager.
     */
    void init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) override;

    /**
     * Overriding function from MemoryManager.
     */
    String getName() override;

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
