/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_MULTIBOOT_H
#define HHUOS_MULTIBOOT_H

#include <cstdint>

#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/base/Exception.h"
#include "lib/util/hardware/Acpi.h"

struct CopyInformation;

namespace Kernel {

class Multiboot {

public:

    enum TagType : uint32_t {
        TERMINATE = 0,
        BOOT_COMMAND_LINE = 1,
        BOOT_LOADER_NAME = 2,
        MODULE = 3,
        BASIC_MEMORY_INFORMATION = 4,
        BIOS_BOOT_DEVICE = 5,
        MEMORY_MAP = 6,
        VBE_INFO = 7,
        FRAMEBUFFER_INFO = 8,
        ELF_SYMBOLS = 9,
        APM_TABLE = 10,
        EFI_32_BIT_SYSTEM_TABLE_POINTER = 11,
        EFI_64_BIT_SYSTEM_TABLE_POINTER = 12,
        SMBIOS_TABLES = 13,
        ACPI_OLD_RSDP = 14,
        ACPI_NEW_RSDP = 15,
        NETWORKING_INFORMATION = 16,
        EFI_MEMORY_MAP = 17,
        EFI_BOOT_SERVICES_NOT_TERMINATED = 18,
        EFI_32_BIT_IMAGE_HANDLE_POINTER = 19,
        EFI_64_BIT_IMAGE_HANDLE_POINTER = 20,
        IMAGE_LOAD_BASE_PHYSICAL_ADDRESS = 21
    };

    enum MemoryType : uint32_t {
        AVAILABLE = 0x01,
        RESERVED = 0x02,
        ACPI_RECLAIMABLE = 0x03,
        NVS = 0x04,
        BAD_RAM = 0x05
    };

    enum FrameBufferType : uint8_t {
        INDEXED = 0,
        RGB = 1,
        EGA_TEXT = 2
    };

    struct Info {
        uint32_t size;
        uint32_t reserved;
    };

    struct TagHeader {
        TagType type;
        uint32_t size;
    };

    struct BootCommandLine {
        TagHeader header;
        const char string[];
    };

    struct BootLoaderName {
        TagHeader header;
        const char string[];
    };

    struct Module {
        TagHeader header;
        uint32_t startAddress;
        uint32_t endAddress;
        const char name[];
    };

    struct BasicMemoryInformation {
        TagHeader header;
        uint32_t lowerMemory;
        uint32_t upperMemory;
    };

    struct BiosBootDevice {
        TagHeader header;
        uint32_t biosDevice;
        uint32_t partition;
        uint32_t subPartition;
    };

    struct MemoryMapHeader {
        TagHeader tagHeader;
        uint32_t entrySize;
        uint32_t entryVersion;
    };

    struct MemoryMapEntry {
        uint64_t address;
        uint64_t length;
        MemoryType type;
        uint32_t reserved;

        bool operator!=(const MemoryMapEntry &other) const {
            return address != other.address;
        }
    };

    struct VbeInfo {
        TagHeader header;
        uint16_t mode;
        uint16_t interfaceSegment;
        uint16_t interfaceOffset;
        uint16_t interfaceLength;
        uint8_t controlInfo[512];
        uint8_t modeInfo[256];
    };

    struct FramebufferPalette {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

    struct IndexedColorInfo {
        uint32_t numColors;
        FramebufferPalette colors[];
    };

    struct RgbColorInfo {
        uint8_t redFieldPosition;
        uint8_t redMaskSize;
        uint8_t greenFieldPosition;
        uint8_t greenMaskSize;
        uint8_t blueFieldPosition;
        uint8_t blueMaskSize;
    };

    union ColorInfo {
        IndexedColorInfo indexInfo;
        RgbColorInfo rgbInfo;
    };

    struct FramebufferInfo {
        TagHeader header;
        uint64_t address;
        uint32_t pitch;
        uint32_t width;
        uint32_t height;
        uint8_t bpp;
        FrameBufferType type;
        uint8_t reserved;
        ColorInfo colorInfo;
    };

    struct ApmTable {
        TagHeader header;
        uint16_t version;
        uint16_t codeSegment;
        uint32_t offset;
        uint16_t codeSegment16;
        uint16_t dataSegment;
        uint16_t flags;
        uint16_t codeSegmentLength;
        uint16_t codeSegment16Length;
        uint16_t dataSegmentLength;
    };

    struct Efi32BitSystemTablePointer {
        TagHeader header;
        uint32_t address;
    };

    struct Efi64BitSystemTablePointer {
        TagHeader header;
        uint64_t address;
    };

    struct SmBiosTables {
        TagHeader header;
        uint8_t majorVersion;
        uint8_t minorVersion;
        uint8_t reserved[6];
        uint8_t tables[];
    };

    struct AcpiRsdp {
        TagHeader header;
        Util::Hardware::Acpi::Rsdp rsdp;
    };

    struct Efi32BitImageHandlePointer {
        TagHeader header;
        uint32_t pointer;
    };

    struct Efi64BitImageHandlePointer {
        TagHeader header;
        uint64_t pointer;
    };

    struct ImageLoadBasePhysicalAddress {
        TagHeader header;
        uint32_t address;
    };

    enum BlockType : uint8_t {
        MULTIBOOT_RESERVED,
        HEAP_RESERVED,
        PAGING_RESERVED
    };

    struct MemoryBlock {
        uint32_t startAddress;
        uint32_t virtualStartAddress;
        uint32_t blockCount;
        bool initialMap;
        BlockType type;
    };

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Multiboot() = delete;

    /**
     * Copy Constructor.
     */
    Multiboot(const Multiboot &other) = delete;

    /**
     * Assignment operator.
     */
    Multiboot &operator=(const Multiboot &other) = delete;

    /**
     * Destructor.
     * Deleted, as this class has only static members.
     */
    ~Multiboot() = delete;

    static void initialize();

    static const CopyInformation& getCopyInformation();

    static Util::String getBootloaderName();

    static FramebufferInfo getFrameBufferInfo();

    static Util::Array<MemoryMapEntry> getMemoryMap();

    static bool hasKernelOption(const Util::String &key);

    static Util::String getKernelOption(const Util::String &key);

    static Util::Array<Util::String> getModuleNames();

    static bool isModuleLoaded(const Util::String &moduleName);

    static const Module& getModule(const Util::String &moduleName);

    static const MemoryBlock* getBlockMap();

    // Used during the bootstrap process

    static void copyMultibootInfo(const Info *source, uint8_t *destination, uint32_t maxBytes);

    static void initializeMemoryBlockMap(const Info *multibootInfo);

    static bool hasTag(TagType type);

    template<typename T>
    static const T& getTag(TagType type);

    static Util::Array<TagType> getAvailableTagTypes();

private:

    static const CopyInformation *copyInformation;
    static const Info *info;

    static const MemoryBlock blockMap[256];
};

template<typename T>
const T& Multiboot::getTag(Multiboot::TagType type) {
    auto currentAddress = reinterpret_cast<uint32_t>(info) + sizeof(Info);
    auto *currentTag = reinterpret_cast<const TagHeader*>(currentAddress);

    while (currentTag->type != TERMINATE) {
        if (currentTag->type == type) {
            return *reinterpret_cast<const T*>(currentTag);
        }

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Tag not found!");
}

}



#endif
