/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __Multiboot_include__
#define __Multiboot_include__

#include <cstdint>
#include "Constants.h"
#include "lib/util/HashMap.h"

namespace Multiboot {

    class Structure {

    public:

        Structure() = delete;

        Structure(const Structure &other) = delete;

        Structure &operator=(const Structure &other) = delete;

        ~Structure() = delete;

        static void parse(Info *address);

        static uint32_t getTotalMem();

        static Multiboot::ModuleInfo getModule(const String &module);

        static bool isModuleLoaded(const String &module);

    private:

        static Info info;

        static MemoryMapEntry *memoryMap;

        static Util::HashMap<String, Multiboot::ModuleInfo> modules;

    };
}



#endif
