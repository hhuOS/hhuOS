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

#include <kernel/memory/MemLayout.h>
#include <kernel/KernelSymbols.h>
#include "Structure.h"
#include "Constants.h"

extern "C" {
    void parse_multiboot(Multiboot::Info *address);
}

void parse_multiboot(Multiboot::Info *address) {

    Multiboot::Structure::parse(address);
}


Multiboot::Info Multiboot::Structure::info;

Multiboot::MemoryMapEntry *Multiboot::Structure::memoryMap;

Util::HashMap<String, Multiboot::ModuleInfo> Multiboot::Structure::modules;

void Multiboot::Structure::parse(Multiboot::Info *address) {

    info = *address;

    memoryMap = (MemoryMapEntry*) (info.memoryMapAddress + KERNEL_START);

    if (info.flags & MULTIBOOT_INFO_ELF_SHDR) {

        info.symbols.elf.address += KERNEL_START;

        KernelSymbols::initialize(info.symbols.elf);
    }

    if (info.flags & MULTIBOOT_INFO_MODS) {

        info.moduleAddress += KERNEL_START;

        Multiboot::ModuleInfo *modInfo = (Multiboot::ModuleInfo*) info.moduleAddress;

        for (uint32_t i = 0; i < info.moduleCount; i++) {

            modInfo[i].string += KERNEL_START;

            modInfo[i].start += KERNEL_START;

            modInfo[i].end += KERNEL_START;

            modules.put(modInfo->string, *modInfo);
        }
    }
}

uint32_t Multiboot::Structure::getTotalMem() {

    MemoryMapEntry *entry = nullptr;

    MemoryMapEntry *best = &memoryMap[0];

    for (uint32_t i = 0; i < info.memoryMapLength / sizeof(MemoryMapEntry); i++) {

        entry = &memoryMap[i];

        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {

            continue;
        }

        if (entry->length > best->length) {

            best = entry;
        }
    }

    return (uint32_t) best->length;
}

Multiboot::ModuleInfo Multiboot::Structure::getModule(const String &module) {

    if (isModuleLoaded(module)) {

        return modules.get(module);
    }

    return {0, 0, "unknown", 0};
}

bool Multiboot::Structure::isModuleLoaded(const String &module) {

    return modules.containsKey(module);
}



