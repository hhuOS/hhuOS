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
#include "Structure.h"

Multiboot::Info Multiboot::Structure::info;

Util::ArrayList<Multiboot::MemoryMapEntry> Multiboot::Structure::memoryMap;

Util::HashMap<String, Multiboot::ModuleInfo> Multiboot::Structure::modules;

Util::ArrayList<VesaGraphics::ModeInfo> Multiboot::Structure::vbeModes;

Util::HashMap<String, String> Multiboot::Structure::kernelOptions;

extern "C" {
    void parse_multiboot(Multiboot::Info *address);
}

void parse_multiboot(Multiboot::Info *address) {

    Multiboot::Structure::parse(address);
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

            memoryMap.add(entry[i]);
        }
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



