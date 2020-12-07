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

#ifndef __Multiboot_include__
#define __Multiboot_include__

#include <cstdint>
#include "lib/string/String.h"
#include "Constants.h"
#include "lib/util/HashMap.h"

#define PRINT_MEMORY 0

namespace Kernel::Multiboot {

    class Structure {

    public:

        struct MemoryBlock {
            uint32_t startAddress;
            uint32_t lengthInBytes;
            uint32_t blockCount;
        };

        Structure() = delete;

        Structure(const Structure &other) = delete;

        Structure &operator=(const Structure &other) = delete;

        ~Structure() = delete;

        static void parse();

        static void init(Multiboot::Info *address);

        static void readMemoryMap(Info *address);

        static Multiboot::ModuleInfo getModule(const String &module);

        static Util::Array<Multiboot::MemoryMapEntry> getMemoryMap();

        static FrameBufferInfo getFrameBufferInfo();

        static bool isModuleLoaded(const String &module);

        static String getKernelOption(const String &key);

        static uint32_t physReservedMemoryEnd;

        static uint32_t physReservedMemoryStart;

        static MemoryBlock blockMap[256];

    private:

        static void parseCommandLine();

        static void parseMemoryMap();

        static void parseSymbols();

        static void parseModules();

        static void parseFrameBufferInfo();

        static Info info;

        static Util::HashMap<String, Multiboot::ModuleInfo> modules;

        static Util::HashMap<String, String> kernelOptions;

        static Util::ArrayList<Multiboot::MemoryMapEntry> memoryMap;

        static MemoryMapEntry customMemoryMap[256];

        static FrameBufferInfo frameBufferInfo;

        static uint32_t customMemoryMapSize;

        static uint32_t kernelCopyLow;

        static uint32_t kernelCopyHigh;
    };
}



#endif
