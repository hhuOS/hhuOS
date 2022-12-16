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
#include "kernel/system/System.h"
#include "kernel/paging/MemoryLayout.h"
#include "Multiboot.h"
#include "kernel/multiboot/Multiboot.h"
#include "asm_interface.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/Exception.h"
#include "lib/util/memory/Address.h"

namespace Kernel {

const Multiboot::Info *Multiboot::info{};
const CopyInformation *Multiboot::copyInformation{};
const Multiboot::MemoryBlock Multiboot::blockMap[256]{};

void Multiboot::initialize() {
    copyInformation = reinterpret_cast<const CopyInformation*>(&multiboot_data);
    info = reinterpret_cast<const Info*>(&multiboot_data + sizeof(CopyInformation));
}

const CopyInformation& Multiboot::getCopyInformation() {
    return *copyInformation;
}

Util::Memory::String Multiboot::getBootloaderName() {
    return reinterpret_cast<const char*>(info->bootloaderName);
}

Multiboot::FrameBufferInfo Multiboot::getFrameBufferInfo() {
    if (info->flags & FRAMEBUFFER_INFO) {
        return info->frameBufferInfo;
    }

    return {};
}

Util::Data::Array<Multiboot::MemoryMapEntry> Multiboot::getMemoryMap() {
    if ((info->flags & MEMORY_MAP) == 0x0) {
        return Util::Data::Array<MemoryMapEntry>(0);
    }

    auto *entry = reinterpret_cast<MemoryMapEntry*>(info->memoryMapAddress);
    uint32_t size = info->memoryMapLength / sizeof(MemoryMapEntry);
    auto memoryMap = Util::Data::Array<MemoryMapEntry>(size);

    for (uint32_t i = 0; i < size; i++) {
        memoryMap[i] = entry[i];
    }

    return memoryMap;
}

const Multiboot::MemoryBlock* Multiboot::getBlockMap() {
    return blockMap;
}

bool Multiboot::hasKernelOption(const Util::Memory::String &key) {
    if (!(info->flags & COMMAND_LINE)) {
        return false;
    }

    Util::Data::Array<Util::Memory::String> options = Util::Memory::String(reinterpret_cast<const char*>(info->commandLine)).split(" ");
    for (const Util::Memory::String &option : options) {
        if (option.split("=")[0] == key) {
            return true;
        }
    }

    return false;
}

Util::Memory::String Multiboot::getKernelOption(const Util::Memory::String &key) {
    if (!(info->flags & COMMAND_LINE)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Requested kernel option is not available!");
    }

    Util::Data::Array<Util::Memory::String> options = Util::Memory::String(reinterpret_cast<const char*>(info->commandLine)).split(" ");
    for (const Util::Memory::String &option : options) {
        auto split = option.split("=");
        if (split[0] == key) {
            return split[1];
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Requested kernel option is not available!");
}

bool Multiboot::isModuleLoaded(const Util::Memory::String &module) {
    if (!(info->flags & MODULES)) {
        return false;
    }

    auto *modInfo = reinterpret_cast<ModuleInfo *>(info->moduleAddress);
    for (uint32_t i = 0; i < info->moduleCount; i++) {
        if (Util::Memory::String(modInfo[i].string) == module) {
            return true;
        }
    }

    return false;
}

Multiboot::ModuleInfo Multiboot::getModule(const Util::Memory::String &module) {
    if (!(info->flags & MODULES)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Requested module is not loaded!");
    }

    auto *modInfo = reinterpret_cast<ModuleInfo*>(info->moduleAddress);
    for (uint32_t i = 0; i < info->moduleCount; i++) {
        if (Util::Memory::String(modInfo[i].string) == module) {
            uint32_t size = modInfo[i].end - modInfo[i].start;
            uint32_t offset = modInfo[i].start % Kernel::Paging::PAGESIZE;

            uint32_t start = reinterpret_cast<uint32_t>(System::getService<Kernel::MemoryService>().mapIO(modInfo[i].start, size)) + offset;
            uint32_t end = start + size;

            return {start, end, modInfo[i].string + Kernel::MemoryLayout::KERNEL_START};
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Requested module is not loaded!");
}

void Multiboot::copyMultibootInfo(const Info *source, uint8_t *destination, uint32_t maxBytes) {
    auto *copyInfo = reinterpret_cast<CopyInformation*>(destination);
    copyInfo->sourceAddress = reinterpret_cast<uint32_t>(source);
    copyInfo->targetAreaSize = maxBytes;
    copyInfo->copiedBytes = sizeof(CopyInformation);
    copyInfo->success = false;

    auto destinationAddress = Util::Memory::Address<uint32_t>(destination + sizeof(CopyInformation));

    // First, copy the struct itself
    if (copyInfo->copiedBytes + sizeof(Info) > maxBytes) return;
    destinationAddress.copyRange(Util::Memory::Address<uint32_t>(source), sizeof(Info));
    auto multibootInfo = reinterpret_cast<Info*>(destinationAddress.get());
    destinationAddress = destinationAddress.add(sizeof(Info));
    copyInfo->copiedBytes += sizeof(Info);

    // Then copy the command line
    if (multibootInfo->flags & COMMAND_LINE) {
        auto sourceAddress = Util::Memory::Address<uint32_t>(multibootInfo->commandLine);
        auto length = sourceAddress.stringLength() + 1;
        if (copyInfo->copiedBytes + length > maxBytes) return;

        destinationAddress.copyString(sourceAddress);
        multibootInfo->commandLine = Kernel::MemoryLayout::PHYSICAL_TO_VIRTUAL(destinationAddress.get());
        destinationAddress = destinationAddress.add(length);
        copyInfo->copiedBytes += length;
    }

    // Then copy the module information
    if (multibootInfo->flags & MODULES) {
        auto sourceAddress = Util::Memory::Address<uint32_t>(multibootInfo->moduleAddress);
        uint32_t length = multibootInfo->moduleCount * sizeof(ModuleInfo);
        if (copyInfo->copiedBytes + length > maxBytes) return;

        destinationAddress.copyRange(sourceAddress, length);
        auto modules = reinterpret_cast<ModuleInfo*>(destinationAddress.get());
        multibootInfo->moduleAddress = Kernel::MemoryLayout::PHYSICAL_TO_VIRTUAL(destinationAddress.get());
        destinationAddress = destinationAddress.add(length);
        copyInfo->copiedBytes += length;

        for(uint32_t i = 0; i < multibootInfo->moduleCount; i++) {
            sourceAddress = Util::Memory::Address<uint32_t>(modules[i].string);
            length = sourceAddress.stringLength() + 1;
            if (copyInfo->copiedBytes + length > maxBytes) return;

            destinationAddress.copyString(sourceAddress);
            modules[i].string = reinterpret_cast<char*>(Kernel::MemoryLayout::PHYSICAL_TO_VIRTUAL(destinationAddress.get()));
            destinationAddress = destinationAddress.add(length);
            copyInfo->copiedBytes += length;
        }
    }

    // Then copy the memory map
    if (multibootInfo->flags & MEMORY_MAP) {
        if (copyInfo->copiedBytes + multibootInfo->memoryMapLength > maxBytes) return;
        auto sourceAddress = Util::Memory::Address<uint32_t>(multibootInfo->memoryMapAddress);
        destinationAddress.copyRange(sourceAddress, multibootInfo->memoryMapLength);
        multibootInfo->memoryMapAddress = Kernel::MemoryLayout::PHYSICAL_TO_VIRTUAL(destinationAddress.get());
        destinationAddress = destinationAddress.add(multibootInfo->memoryMapLength);
        copyInfo->copiedBytes += multibootInfo->memoryMapLength;
    }

    // Then copy the drives
    if (multibootInfo -> flags & DRIVE_INFO) {
        if (copyInfo->copiedBytes + multibootInfo->driveLength > maxBytes) return;
        auto sourceAddress = Util::Memory::Address<uint32_t>(multibootInfo->driveAddress);
        destinationAddress.copyRange(sourceAddress, multibootInfo->driveLength);
        multibootInfo->driveAddress = Kernel::MemoryLayout::PHYSICAL_TO_VIRTUAL(destinationAddress.get());
        destinationAddress = destinationAddress.add(multibootInfo->driveLength);
        copyInfo->copiedBytes += multibootInfo->driveLength;
    }

    // Then copy the bootloader name
    if (multibootInfo->flags & BOOT_LOADER_NAME) {
        auto sourceAddress = Util::Memory::Address<uint32_t>(multibootInfo->bootloaderName);
        auto length = sourceAddress.stringLength() + 1;
        if (copyInfo->copiedBytes + length > maxBytes) return;
        destinationAddress.copyString(sourceAddress);
        multibootInfo->bootloaderName = Kernel::MemoryLayout::PHYSICAL_TO_VIRTUAL(destinationAddress.get());
        copyInfo->copiedBytes += length;
    }

    copyInfo->success = true;
}

void Multiboot::readMemoryMap(const Info *multibootInfo) {
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

    if (multibootInfo->flags & MODULES) {
        auto *modInfo = reinterpret_cast<const ModuleInfo*>(multibootInfo->moduleAddress);
        for (uint32_t i = 0; i < multibootInfo->moduleCount; i++) {
            uint32_t alignedStartAddress = (modInfo[i].start / alignment) * alignment;
            uint32_t alignedEndAddress = modInfo[i].end % alignment == 0 ? modInfo[i].end : (modInfo[i].end / alignment) * alignment + alignment;
            uint32_t blockCount = (alignedEndAddress - alignedStartAddress) / alignment;

            blocks[blockIndex++] = { alignedStartAddress, 0, blockCount, false, MULTIBOOT_RESERVED };
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

}
