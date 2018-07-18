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
#include <lib/file/tar/Archive.h>
#include <kernel/memory/SystemManagement.h>
#include <lib/libc/printf.h>
#include <lib/libc/sprintf.h>
#include <kernel/interrupts/BlueScreen.h>
#include "Structure.h"
#include "Constants.h"

extern "C" {
    extern char ___KERNEL_START__;
    extern char ___KERNEL_END__;
}

Multiboot::Info Multiboot::Structure::info;

uint32_t Multiboot::Structure::customMemoryMapSize = 0;

uint32_t Multiboot::Structure::reservedMemoryStart = UINT32_MAX;

uint32_t Multiboot::Structure::reservedMemoryEnd = 0;

uint32_t Multiboot::Structure::kernelCopyLow = UINT32_MAX;

uint32_t Multiboot::Structure::kernelCopyHigh = 0;

Multiboot::MemoryMapEntry Multiboot::Structure::customMemoryMap[256];

Util::ArrayList<Multiboot::MemoryMapEntry> Multiboot::Structure::memoryMap;

Util::HashMap<String, Multiboot::ModuleInfo> Multiboot::Structure::modules;

Util::ArrayList<VesaGraphics::ModeInfo> Multiboot::Structure::vbeModes;

Util::HashMap<String, String> Multiboot::Structure::kernelOptions;

extern "C" {
    void parse_multiboot(Multiboot::Info *address);
    void readMemoryMap(Multiboot::Info *address);
}

void parse_multiboot(Multiboot::Info *address) {

    Multiboot::Structure::parse(address);
}

void readMemoryMap(Multiboot::Info *address) {

    Multiboot::Structure::readMemoryMap(address);
}

void Multiboot::Structure::readMemoryMap(Multiboot::Info *address) {

    Multiboot::Info tmp = *address;

    MemoryMapEntry *memory = (MemoryMapEntry*) ((uint32_t) customMemoryMap - KERNEL_START);

    uint32_t &mapSize = *((uint32_t*) ((uint32_t) &customMemoryMapSize - KERNEL_START));

    uint32_t kernelStart = (uint32_t) &___KERNEL_START__ - KERNEL_START;

    uint32_t kernelEnd = (uint32_t) &___KERNEL_END__ - KERNEL_START;

    memory[0] = {0x0, kernelStart, kernelEnd - kernelStart, MULTIBOOT_MEMORY_RESERVED};

    mapSize++;

    uint32_t memoryIndex = 1;

    ElfInfo &symbolInfo = tmp.symbols.elf;

    ElfConstants::SectionHeader *sectionHeader = nullptr;
    
    if (tmp.flags & *VIRT2PHYS(&MULTIBOOT_INFO_ELF_SHDR)) {

        for (uint32_t i = 0; i < symbolInfo.sectionCount; i++) {

            sectionHeader = (ElfConstants::SectionHeader*) (symbolInfo.address + i * symbolInfo.sectionSize);

            if (sectionHeader->virtualAddress == 0x0) {

                continue;
            }

            uint32_t startAddress = sectionHeader->virtualAddress < KERNEL_START ? sectionHeader->virtualAddress : sectionHeader->virtualAddress - KERNEL_START;

            memory[memoryIndex] = {0x0, startAddress, sectionHeader->size, MULTIBOOT_MEMORY_RESERVED,};

            memoryIndex++;

            mapSize++;

            // Copy string and symbol table so it won't get overridden
            switch (sectionHeader->type) {
                case ElfConstants::SectionHeaderType::STRTAB:
                    memcpy((char*) PHYS_STRTAB, (char*) startAddress, sectionHeader->size);
                    break;
                case ElfConstants::SectionHeaderType::SYMTAB:
                    memcpy((char*) PHYS_SYMTAB, (char*) startAddress, sectionHeader->size);
                    break;
                default:
                    break;
            }
        }
    }

    if (tmp.flags & *VIRT2PHYS(&MULTIBOOT_INFO_MODS)) {

        auto *modInfo = (Multiboot::ModuleInfo*) tmp.moduleAddress;

        for (uint32_t i = 0; i < tmp.moduleCount; i++) {

            memory[memoryIndex] = {0x0, modInfo[i].start, modInfo[i].end - modInfo[i].start, MULTIBOOT_MEMORY_RESERVED,};

            memoryIndex++;

            mapSize++;
        }
    }

    uint32_t &maxAddress = *((uint32_t*) ((uint32_t) &reservedMemoryEnd - KERNEL_START));

    uint32_t &minAddress = *((uint32_t*) ((uint32_t) &reservedMemoryStart - KERNEL_START));

    for (uint32_t i = 0; i < memoryIndex; i++) {

        if (memory[i].address + memory[i].length > maxAddress) {

            maxAddress = memory[i].address + memory[i].length;
        }

        if (memory[i].address < minAddress) {

            minAddress = memory[i].address;
        }
    }
}

void Multiboot::Structure::parse(Multiboot::Info *address) {

    info = *address;

    parseCommandLine();

    parseMemoryMap();

    parseSymbols();

    parseModules();

    parseVbeInfo();
}

void Multiboot::Structure::parseCommandLine() {

    if (info.flags & MULTIBOOT_INFO_CMDLINE) {

        info.commandLine += KERNEL_START;

        Util::Array<String> options = String((char*) info.commandLine).split(" ");

        for (const String &option : options) {

            Util::Array<String> pair = option.split("=");

            if (pair.length() != 2) {

                continue;
            }

            kernelOptions.put(pair[0], pair[1]);
        }
    }
}

void Multiboot::Structure::parseMemoryMap() {

    if (info.flags & MULTIBOOT_INFO_MEM_MAP) {

        MemoryMapEntry *entry = (MemoryMapEntry*) (info.memoryMapAddress + KERNEL_START);

        uint32_t size = info.memoryMapLength / sizeof(MemoryMapEntry);

        for (uint32_t i = 0; i < size; i++) {

#if PRINT_MEMORY
            printf("0x%08x - 0x%08x : %u\n", (uint32_t) entry[i].address, (uint32_t) (entry[i].address + entry[i].length), entry[i].type);
#endif

            memoryMap.add(entry[i]);
        }

#if PRINT_MEMORY
        while(true);
#endif
    }
}

void Multiboot::Structure::parseSymbols() {

    if (info.flags & MULTIBOOT_INFO_ELF_SHDR) {

        info.symbols.elf.address += KERNEL_START;

        KernelSymbols::initialize(info.symbols.elf);
    }
}

void Multiboot::Structure::parseModules() {

    if (info.flags & MULTIBOOT_INFO_MODS) {

        info.moduleAddress += KERNEL_START;

        Multiboot::ModuleInfo *modInfo = (Multiboot::ModuleInfo*) info.moduleAddress;

        for (uint32_t i = 0; i < info.moduleCount; i++) {

            SystemManagement::getInstance()->reservePhysicalMemory(modInfo[i].start, modInfo[i].end);

            modInfo[i].string += KERNEL_START;

            modInfo[i].start += KERNEL_START;

            modInfo[i].end += KERNEL_START;

            modules.put(modInfo->string, *modInfo);
        }
    }
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

String Multiboot::Structure::getKernelOption(const String &key) {

    if (kernelOptions.containsKey(key)) {

        return kernelOptions.get(key);
    }

    return String();
}

void Multiboot::Structure::parseVbeInfo() {

    if (info.flags & MULTIBOOT_INFO_VBE_INFO) {

        info.vbeModeInfo += KERNEL_START;

        VesaGraphics::ModeInfo *modeInfo = (VesaGraphics::ModeInfo*) info.vbeModeInfo;

        vbeModes.add(*modeInfo);

        // TODO(krakowski)
        //  Save VBE control information and get available modes
    }
}



