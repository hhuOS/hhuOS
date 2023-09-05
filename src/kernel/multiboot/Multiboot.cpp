/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "lib/util/base/Exception.h"
#include "lib/util/base/Address.h"
#include "lib/util/collection/ArrayList.h"

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

Util::String Multiboot::getBootloaderName() {
    if (!hasTag(BOOT_LOADER_NAME)) {
        return "";
    }

    return getTag<BootLoaderName>(BOOT_LOADER_NAME).string;
}

Multiboot::FramebufferInfo Multiboot::getFrameBufferInfo() {
    if (!hasTag(FRAMEBUFFER_INFO)) {
        return {};
    }

    return getTag<FramebufferInfo>(FRAMEBUFFER_INFO);
}

Util::Array<Multiboot::MemoryMapEntry> Multiboot::getMemoryMap() {
    if (!hasTag(MEMORY_MAP)) {
        return Util::Array<MemoryMapEntry>(0);
    }

    const auto &memoryMapHeader = getTag<MemoryMapHeader>(MEMORY_MAP);
    auto numEntries = (memoryMapHeader.tagHeader.size - sizeof(TagHeader)) / memoryMapHeader.entrySize;
    auto memoryMap = Util::Array<MemoryMapEntry>(numEntries);

    for (uint32_t i = 0; i < numEntries; i++) {
        auto currentAddress = reinterpret_cast<uint32_t>(&memoryMapHeader) + sizeof(MemoryMapHeader) + i * memoryMapHeader.entrySize;
        const auto &entry = *reinterpret_cast<const MemoryMapEntry*>(currentAddress);
        memoryMap[i] = entry;
    }

    return memoryMap;
}

bool Multiboot::hasKernelOption(const Util::String &key) {
    if (!hasTag(BOOT_COMMAND_LINE)) {
        return false;
    }

    auto commandLine = getTag<BootCommandLine>(BOOT_COMMAND_LINE).string;
    Util::Array<Util::String> options = Util::String(commandLine).split(" ");

    for (const Util::String &option : options) {
        if (option.split("=")[0] == key) {
            return true;
        }
    }

    return false;
}

Util::String Multiboot::getKernelOption(const Util::String &key) {
    if (!hasTag(BOOT_COMMAND_LINE)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Kernel options are not available!");
    }

    auto commandLine = getTag<BootCommandLine>(BOOT_COMMAND_LINE).string;
    Util::Array<Util::String> options = Util::String(commandLine).split(" ");

    for (const Util::String &option : options) {
        auto split = option.split("=");
        if (split[0] == key) {
            return split[1];
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Requested kernel option is not available!");
}

bool Multiboot::isModuleLoaded(const Util::String &moduleName) {
    auto currentAddress = reinterpret_cast<uint32_t>(info) + sizeof(Info);
    auto *currentTag = reinterpret_cast<const TagHeader*>(currentAddress);

    while (currentTag->type != TERMINATE) {
        if (currentTag->type == MODULE) {
            auto *module = reinterpret_cast<const Module*>(currentTag);
            if (moduleName == module->name) {
                return true;
            }
        }

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    }

    return false;
}

const Multiboot::Module& Multiboot::getModule(const Util::String &moduleName) {
    auto currentAddress = reinterpret_cast<uint32_t>(info) + sizeof(Info);
    auto *currentTag = reinterpret_cast<const TagHeader*>(currentAddress);

    while (currentTag->type != TERMINATE) {
        if (currentTag->type == MODULE) {
            auto *module = reinterpret_cast<Module*>(const_cast<TagHeader*>(currentTag));
            if (moduleName == module->name) {
                if (module->startAddress < MemoryLayout::KERNEL_START) {
                    uint32_t size = module->endAddress - module->startAddress;
                    uint32_t offset = module->startAddress % Kernel::Paging::PAGESIZE;

                    uint32_t virtualStartAddress = reinterpret_cast<uint32_t>(System::getService<Kernel::MemoryService>().mapIO(module->startAddress, size)) + offset;
                    uint32_t virtualEndAddress = virtualStartAddress + size;

                    module->startAddress = virtualStartAddress;
                    module->endAddress = virtualEndAddress;
                }

                return *module;
            }
        }

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Requested module is not loaded!");
}

void Multiboot::copyMultibootInfo(const Info *source, uint8_t *destination, uint32_t maxBytes) {
    auto *copyInfo = reinterpret_cast<CopyInformation*>(destination);
    copyInfo->sourceAddress = reinterpret_cast<uint32_t>(source);
    copyInfo->targetAreaSize = maxBytes;
    copyInfo->copiedBytes = sizeof(CopyInformation);
    copyInfo->success = true;

    uint32_t toCopy = source->size;
    if (copyInfo->copiedBytes + source->size > maxBytes) {
        toCopy = maxBytes - copyInfo->copiedBytes;
        copyInfo->success = false;
    }

    auto destinationAddress = Util::Address<uint32_t>(destination + sizeof(CopyInformation));
    destinationAddress.copyRange(Util::Address<uint32_t>(source), toCopy);
    copyInfo->copiedBytes += toCopy;
}

const Multiboot::MemoryBlock* Multiboot::getBlockMap() {
    return blockMap;
}

void Multiboot::initializeMemoryBlockMap(const Info *multibootInfo) {
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

    // Search for modules
    auto currentAddress = reinterpret_cast<uint32_t>(multibootInfo) + sizeof(Info);
    auto *currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    while (currentTag->type != TERMINATE) {
        if (currentTag->type == MODULE) {
            auto *moduleTag = reinterpret_cast<const Module*>(currentTag);
            uint32_t alignedStartAddress = (moduleTag->startAddress / alignment) * alignment;
            uint32_t alignedEndAddress = moduleTag->endAddress % alignment == 0 ? moduleTag->endAddress : (moduleTag->endAddress / alignment) * alignment + alignment;
            uint32_t blockCount = (alignedEndAddress - alignedStartAddress) / alignment;

            blocks[blockIndex++] = { alignedStartAddress, 0, blockCount, false, MULTIBOOT_RESERVED };
        }

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    }

    // Sort block map
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

bool Multiboot::hasTag(Multiboot::TagType type) {
    auto currentAddress = reinterpret_cast<uint32_t>(info) + sizeof(Info);
    auto *currentTag = reinterpret_cast<const TagHeader*>(currentAddress);

    while (currentTag->type != TERMINATE) {
        if (currentTag->type == type) {
            return true;
        }

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    }

    return false;
}

Util::Array<Multiboot::TagType> Multiboot::getAvailableTagTypes() {
    Util::ArrayList<TagType> types;
    auto currentAddress = reinterpret_cast<uint32_t>(info) + sizeof(Info);
    auto *currentTag = reinterpret_cast<const TagHeader*>(currentAddress);

    while (currentTag->type != TERMINATE) {
        types.add(currentTag->type);

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    }

    return types.toArray();
}

}
