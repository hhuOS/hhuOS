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

#ifndef __IOMEMMANAGER_H__
#define __IOMEMMANAGER_H__

#include <cstdint>
#include "lib/util/HashMap.h"
#include "lib/async/Spinlock.h"
#include "BitmapMemoryManager.h"

namespace Kernel {

/**
 * Memory manager, that manages IO-Space for HW-Buffers, DMA etc.
 * Maps a given physical address (for example LFB) into the virtual IO-space
 * as defined in MemLayout.h or allocates an 4kb-aligned physical buffer and
 * maps it into virtual IO-space.
 * It is based on the BitmapMemoryManager.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date 2018
 */
class IOMemoryManager : public BitmapMemoryManager {

private:

    Util::HashMap<void *, uint32_t> ioMemoryMap;

    Spinlock lock;

    static const constexpr char *TYPE_NAME = "IOMemoryManager";

public:

    PROTOTYPE_IMPLEMENT_CLONE(IOMemoryManager);

    /**
     * Constructor.
     */
    IOMemoryManager();

    /**
     * Copy-constructor.
     */
    IOMemoryManager(const IOMemoryManager &copy);

    /**
     * Destructor.
     */
    ~IOMemoryManager() override = default;

    /**
     * Overriding function from MemoryManager.
     */
    void init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) override;

    /**
     * Overriding function from MemoryManager.
     */
    String getTypeName() override;

    /**
     * Overriding function from MemoryManager.
     */
    void *alloc(uint32_t size) override;

    /**
     * Overriding function from MemoryManager.
     */
    void free(void *ptr) override;
};

}

#endif
