/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "kernel/paging/Paging.h"
#include "asm_interface.h"
#include "kernel/system/System.h"
#include "kernel/service/MemoryService.h"
#include "kernel/paging/MemoryLayout.h"
#include "kernel/system/Symbols.h"
#include "Structure.h"
#include "Constants.h"

namespace Kernel::Multiboot {

Info Structure::info;
Structure::MemoryBlock Structure::blockMap[256];
Util::Data::ArrayList<MemoryMapEntry> Structure::memoryMap;
FrameBufferInfo Structure::frameBufferInfo;
Util::Data::HashMap<Util::Memory::String, ModuleInfo> Structure::modules;
Util::Data::HashMap<Util::Memory::String, Util::Memory::String> Structure::kernelOptions;

void Structure::initialize(Info *address) {
    info = *address;
}

void Structure::parse() {
    parseCommandLine();
    parseMemoryMap();
    parseSymbols();
    parseModules();
    parseFrameBufferInfo();
}

void Structure::parseCommandLine() {
    if (info.flags & MULTIBOOT_INFO_CMDLINE) {
        info.commandLine += Kernel::MemoryLayout::KERNEL_START;
        Util::Data::Array<Util::Memory::String> options = Util::Memory::String((char *) info.commandLine).split(" ");

        for (const Util::Memory::String &option : options) {
            Util::Data::Array<Util::Memory::String> pair = option.split("=");
            if (pair.length() != 2) {
                continue;
            }

            kernelOptions.put(pair[0], pair[1]);
        }
    }
}

void Structure::parseMemoryMap() {
    if (info.flags & MULTIBOOT_INFO_MEM_MAP) {
        auto *entry = reinterpret_cast<MemoryMapEntry*>(info.memoryMapAddress + Kernel::MemoryLayout::KERNEL_START);
        uint32_t size = info.memoryMapLength / sizeof(MemoryMapEntry);

        for (uint32_t i = 0; i < size; i++) {
            memoryMap.add(entry[i]);
        }
    }
}

Util::Data::Array<MemoryMapEntry> Structure::getMemoryMap() {
    if ((info.flags & MULTIBOOT_INFO_MEM_MAP) == 0x0) {
        return Util::Data::Array<MemoryMapEntry>(0);
    }

    auto *entry = reinterpret_cast<MemoryMapEntry*>(info.memoryMapAddress + Kernel::MemoryLayout::KERNEL_START);
    uint32_t size = info.memoryMapLength / sizeof(MemoryMapEntry);
    Util::Data::Array<MemoryMapEntry> copy(size);

    for (uint32_t i = 0; i < size; i++) {
        copy[i] = entry[i];
    }

    return copy;
}

FrameBufferInfo Structure::getFrameBufferInfo() {
    return frameBufferInfo;
}

void Structure::parseSymbols() {
    if (info.flags & MULTIBOOT_INFO_ELF_SHDR) {
        info.symbols.elf.address += Kernel::MemoryLayout::KERNEL_START;
        Symbols::initialize(info.symbols.elf);
    }
}

void Structure::parseModules() {
    if (info.flags & MULTIBOOT_INFO_MODS) {
        info.moduleAddress += Kernel::MemoryLayout::KERNEL_START;
        auto *modInfo = reinterpret_cast<ModuleInfo *>(info.moduleAddress);

        for (uint32_t i = 0; i < info.moduleCount; i++) {
            uint32_t size = modInfo[i].end - modInfo[i].start;
            uint32_t offset = modInfo[i].start % Kernel::Paging::PAGESIZE;

            modInfo[i].string += Kernel::MemoryLayout::KERNEL_START;
            modInfo[i].start = reinterpret_cast<uint32_t>(System::getService<Kernel::MemoryService>().mapIO(modInfo[i].start, size)) + offset;
            modInfo[i].end += modInfo[i].start + size;

            modules.put(modInfo->string, *modInfo);
        }
    }
}

ModuleInfo Structure::getModule(const Util::Memory::String &module) {
    if (isModuleLoaded(module)) {
        return modules.get(module);
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Request module is not loaded!");
}

bool Structure::isModuleLoaded(const Util::Memory::String &module) {
    return modules.containsKey(module);
}

bool Structure::hasKernelOption(const Util::Memory::String &key) {
    return kernelOptions.containsKey(key);
}

Util::Memory::String Structure::getKernelOption(const Util::Memory::String &key) {
    if (kernelOptions.containsKey(key)) {
        return kernelOptions.get(key);
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Request kernel option is not available!");
}

void Structure::parseFrameBufferInfo() {
    if (info.flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO) {
        frameBufferInfo.address = static_cast<uint32_t>(info.framebufferAddress);
        frameBufferInfo.width = static_cast<uint16_t>(info.framebufferWidth);
        frameBufferInfo.height = static_cast<uint16_t>(info.framebufferHeight);
        frameBufferInfo.bpp = info.framebufferBpp;
        frameBufferInfo.pitch = static_cast<uint16_t>(info.framebufferPitch);
        frameBufferInfo.type = info.framebufferType;
    } else {
        frameBufferInfo.address = 0;
        frameBufferInfo.width = 0;
        frameBufferInfo.height = 0;
        frameBufferInfo.bpp = 0;
        frameBufferInfo.pitch = 0;
        frameBufferInfo.type = 0;
    }
}

void Structure::copyMultibootInfo(Info *source, uint8_t *destination, uint32_t maxBytes) {
    auto destinationAddress = Util::Memory::Address<uint32_t>(destination);

    // First, copy the struct itself
    destinationAddress.copyRange(Util::Memory::Address<uint32_t>(source), sizeof(Info));
    auto multibootInfo = reinterpret_cast<Info *>(destinationAddress.get());
    destinationAddress = destinationAddress.add(sizeof(Info));

    // Then copy the command line
    if (multibootInfo->flags & MULTIBOOT_INFO_CMDLINE) {
        auto sourceAddress = Util::Memory::Address<uint32_t>(multibootInfo->commandLine);
        destinationAddress.copyString(sourceAddress);
        multibootInfo->commandLine = destinationAddress.get();
        destinationAddress = destinationAddress.add(sourceAddress.stringLength() + 1);
    }

    // Then copy the module information
    if (multibootInfo->flags & MULTIBOOT_INFO_MODS) {
        uint32_t length = multibootInfo->moduleCount * sizeof(ModuleInfo);
        auto sourceAddress = Util::Memory::Address<uint32_t>(multibootInfo->moduleAddress);
        destinationAddress.copyRange(sourceAddress, length);
        multibootInfo->moduleAddress = destinationAddress.get();
        destinationAddress = destinationAddress.add(length);

        auto mods = reinterpret_cast<ModuleInfo*>(multibootInfo->moduleAddress);
        for(uint32_t i = 0; i < multibootInfo->moduleCount; i++) {
            sourceAddress = Util::Memory::Address<uint32_t>(mods[i].string);
            destinationAddress.copyString(sourceAddress);
            mods[i].string = reinterpret_cast<char*>(destinationAddress.get());
            destinationAddress = destinationAddress.add(sourceAddress.stringLength() + 1);
        }
    }

    // TODO: For some reason, the system won't boot, when this code is active
    // Then copy the symbol headers and the symbols
    if (multibootInfo->flags & MULTIBOOT_INFO_ELF_SHDR) {
        uint32_t length = multibootInfo->symbols.elf.sectionSize * multibootInfo->symbols.elf.sectionCount;
        auto sourceAddress = Util::Memory::Address<uint32_t>(multibootInfo->symbols.elf.address);
        destinationAddress.copyRange(sourceAddress, length);
        multibootInfo->symbols.elf.address = destinationAddress.get();
        destinationAddress = destinationAddress.add(length);
        destinationAddress = Symbols::copy(multibootInfo->symbols.elf, destinationAddress);
    }

    // Then copy the memory map
    if (multibootInfo->flags & MULTIBOOT_INFO_MEM_MAP) {
        auto sourceAddress = Util::Memory::Address<uint32_t>(multibootInfo->memoryMapAddress);
        destinationAddress.copyRange(sourceAddress, multibootInfo->memoryMapLength);
        multibootInfo->memoryMapAddress = destinationAddress.get();
        destinationAddress = destinationAddress.add(multibootInfo->memoryMapLength);
    }

    // Then copy the drives
    if (multibootInfo -> flags & MULTIBOOT_INFO_DRIVE_INFO) {
        auto sourceAddress = Util::Memory::Address<uint32_t>(multibootInfo->driveAddress);
        destinationAddress.copyRange(sourceAddress, multibootInfo->driveLength);
        multibootInfo->driveAddress = destinationAddress.get();
        destinationAddress = destinationAddress.add(multibootInfo->driveLength);
    }

    // Then copy the bootloader name
    if (multibootInfo->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) {
        auto sourceAddress = Util::Memory::Address<uint32_t>(multibootInfo->bootloaderName);
        destinationAddress.copyString(sourceAddress);
        multibootInfo->bootloaderName = destinationAddress.get();
        destinationAddress = destinationAddress.add(sourceAddress.stringLength());
    }
}

void Structure::readMemoryMap(Info *address) {
    Info info = *address;

    auto *blocks = reinterpret_cast<MemoryBlock*>(MemoryLayout::VIRTUAL_TO_PHYSICAL(reinterpret_cast<uint32_t>(blockMap)));
    auto kernelStart = reinterpret_cast<uint32_t>(MemoryLayout::VIRTUAL_TO_PHYSICAL(reinterpret_cast<uint32_t>(&___KERNEL_DATA_START__)));
    auto kernelEnd = reinterpret_cast<uint32_t>(MemoryLayout::VIRTUAL_TO_PHYSICAL(reinterpret_cast<uint32_t>(&___KERNEL_DATA_END__)));

    uint32_t alignment = 4 * 1024 * 1024;
    uint32_t kernelStartAligned = (kernelStart / alignment) * alignment;
    uint32_t kernelEndAligned = kernelEnd % alignment == 0 ? kernelEnd : (kernelEnd / alignment) * alignment + alignment;
    uint32_t blockIndex = 0;

    // Add kernel blocks
    blocks[blockIndex++] = { kernelStartAligned, 0, (kernelEndAligned - kernelStartAligned) / alignment, true, MULTIBOOT_RESERVED };
    // Add lower memory blocks
    blocks[blockIndex++] = { 0, 0, 256, false, MULTIBOOT_RESERVED };

    alignment = 4 * 1024;

    if (info.flags & MULTIBOOT_INFO_MODS) {
        auto *modInfo = (ModuleInfo *) info.moduleAddress;

        for (uint32_t i = 0; i < info.moduleCount; i++) {
            uint32_t alignedStartAddress = (modInfo[i].start / alignment) * alignment;
            uint32_t alignedEndAddress = modInfo[i].end % alignment == 0 ? modInfo[i].end : (modInfo[i].end / alignment) * alignment + alignment;
            uint32_t blockCount = (alignedEndAddress - alignedStartAddress) / alignment;

            blocks[blockIndex++] = {alignedStartAddress, 0, blockCount, false, MULTIBOOT_RESERVED };
        }
    }

    bool sorted;
    do {
        sorted = true;
        for (uint32_t i = 0; i < blockIndex - 1; i++) {
            if (blocks[i].startAddress > blocks[i + 1].startAddress) {
                const auto help = blocks[i];
                blocks[i] = blocks[i + 1];
                blocks[i + 1] = help;

                sorted = false;
            }
        }
    } while (!sorted);

    // Merge consecutive blocks
    for (uint32_t i = 0; i < blockIndex;) {
        // initialMap -> 4 MiB granularity; else -> 4 KiB granularity
        if (blocks[i].initialMap && blocks[i + 1].initialMap) {
            if (blocks[i].startAddress + blocks[i].blockCount * Kernel::Paging::PAGESIZE * 1024 >= blocks[i + 1].startAddress) {
                uint32_t firstEndAddress = blocks[i].startAddress + blocks[i].blockCount * Kernel::Paging::PAGESIZE * 1024;
                uint32_t secondEndAddress = blocks[i + 1].startAddress + blocks[i + 1].blockCount * Kernel::Paging::PAGESIZE * 1024;
                uint32_t endAddress = firstEndAddress > secondEndAddress ? firstEndAddress : secondEndAddress;
                blocks[i].blockCount = (endAddress - blocks[i].startAddress) / (Kernel::Paging::PAGESIZE * 1024);

                // Shift remaining blocks to close gap
                for (uint32_t j = i + 1; j < blockIndex; j++) {
                    blocks[j] = blocks[j + 1];
                }

                blocks[blockIndex] = { 0, 0, 0, false, MULTIBOOT_RESERVED };
                blockIndex--;
            }
        } else {
            i++;
        }
    }
}

Structure::MemoryBlock *Structure::getBlockMap() {
    return blockMap;
}

}
