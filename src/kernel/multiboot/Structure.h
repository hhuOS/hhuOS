/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef __Multiboot_include__
#define __Multiboot_include__

#include <cstdint>
#include "Constants.h"
#include "lib/util/memory/String.h"
#include "lib/util/data/HashMap.h"

namespace Kernel::Multiboot {

    class Structure {

    public:

        static uint32_t usedBytes;

        enum BlockType : uint8_t {
            MULTIBOOT_RESERVED = 0x00,
            HEAP_RESERVED = 0x01,
            PAGING_RESERVED = 0x02
        };

        struct MemoryBlock {
            uint32_t startAddress;
            uint32_t virtualStartAddress;
            uint32_t blockCount;
            BlockType type;
        };

        Structure() = delete;

        Structure(const Structure &other) = delete;

        Structure &operator=(const Structure &other) = delete;

        ~Structure() = delete;

        static void copyMultibootInfo(Info *source, uint8_t *destination, uint32_t maxBytes);

        static void readMemoryMap(Info *address);

        static void parse();

        static void init(Multiboot::Info *address);

        static Multiboot::ModuleInfo getModule(const Util::Memory::String &module);

        static Util::Data::Array<Multiboot::MemoryMapEntry> getMemoryMap();

        static FrameBufferInfo getFrameBufferInfo();

        static bool isModuleLoaded(const Util::Memory::String &module);

        static bool hasKernelOption(const Util::Memory::String &key);

        static Util::Memory::String getKernelOption(const Util::Memory::String &key);

        static MemoryBlock blockMap[256];

        static Info info;

    private:

        static void parseCommandLine();

        static void parseMemoryMap();

        static void parseSymbols();

        static void parseModules();

        static void parseFrameBufferInfo();

        static Util::Data::HashMap<Util::Memory::String, Multiboot::ModuleInfo> modules;

        static Util::Data::HashMap<Util::Memory::String, Util::Memory::String> kernelOptions;

        static Util::Data::ArrayList<Multiboot::MemoryMapEntry> memoryMap;

        static MemoryMapEntry customMemoryMap[256];

        static FrameBufferInfo frameBufferInfo;

        static uint32_t customMemoryMapSize;
    };
}



#endif
