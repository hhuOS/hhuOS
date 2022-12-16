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

#ifndef HHUOS_MULTIBOOT_H
#define HHUOS_MULTIBOOT_H

#include <cstdint>

#include "lib/util/memory/String.h"
#include "lib/util/data/Array.h"

struct CopyInformation;

namespace Kernel {

class Multiboot {

public:

    enum FrameBufferType : uint8_t {
        INDEXED = 0,
        RGB = 1,
        EGA_TEXT = 2
    };

    enum MultibootInfoFlag : uint32_t {
        MEMORY = 0x00000001,
        BOOT_DEVICE = 0x00000002,
        COMMAND_LINE = 0x00000004,
        MODULES = 0x00000008,
        AOUT_SYMBOL = 0x00000010,
        ELF_SECTION_HEADER = 0x00000020,
        MEMORY_MAP = 0x00000040,
        DRIVE_INFO = 0x00000080,
        CONFIGURATION_TABLE = 0x00000100,
        BOOT_LOADER_NAME = 0x00000200,
        APM_TABLE = 0x00000400,
        VBE_INFO = 0x00000800,
        FRAMEBUFFER_INFO = 0x00001000
    };

    enum MemoryType : uint32_t {
        AVAILABLE = 0x01,
        RESERVED = 0x02,
        ACPI_RECLAIMABLE = 0x03,
        NVS = 0x04,
        BAD_RAM = 0x05
    };

    struct AoutInfo {
        uint32_t tabSize;
        uint32_t strsize;
        uint32_t address;
        uint32_t reserved;
    };

    struct ElfInfo {
        uint32_t sectionCount;
        uint32_t sectionSize;
        uint32_t address;
        uint32_t stringSectionIndex;
    };

    struct ModuleInfo {
        uint32_t start;
        uint32_t end;
        const char *string;
        uint32_t reserved;

        bool operator!=(const ModuleInfo &other) {

            return start != other.start;
        }
    };

    struct FrameBufferInfo {
        uint64_t address;
        uint32_t pitch;
        uint32_t width;
        uint32_t height;
        uint8_t bpp;
        FrameBufferType type;
    };

    struct MemoryMapEntry {
        uint32_t size;
        uint64_t address;
        uint64_t length;
        MemoryType type;

        bool operator!=(const MemoryMapEntry &other) const {
            return address != other.address;
        }
    };

    struct Info {
        /* Multiboot info version number */
        uint32_t flags;

        /* Available memory from BIOS */
        uint32_t memoryLower;
        uint32_t memoryUpper;

        /* "root" partition */
        uint32_t bootDevice;

        /* Kernel command line */
        uint32_t commandLine;

        /* Boot-Module list */
        uint32_t moduleCount;
        uint32_t moduleAddress;

        /* Symbol information */
        union {
            AoutInfo aout;
            ElfInfo elf;
        } symbols;

        /* Memory Mapping buffer */
        uint32_t memoryMapLength;
        uint32_t memoryMapAddress;

        /* Drive Info buffer */
        uint32_t driveLength;
        uint32_t driveAddress;

        /* ROM configuration table */
        uint32_t configTable;

        /* Boot Loader Name */
        uint32_t bootloaderName;

        /* APM table */
        uint32_t apmTable;

        /* Video */
        uint32_t vbeControlInfo;
        uint32_t vbeModeInfo;
        uint16_t vbeMode;
        uint16_t vbeInterfaceSegment;
        uint16_t vbeInterfaceOffset;
        uint16_t vbeInterfaceLength;

        FrameBufferInfo frameBufferInfo;

        union {

            struct {
                uint32_t address;
                uint16_t colors;
            } palette;

            struct {
                uint8_t redFieldPosition;
                uint8_t redMaskSize;
                uint8_t greenFieldPosition;
                uint8_t greenMaskSize;
                uint8_t blueFieldPosition;
                uint8_t blueMaskSize;
            } color;

        } framebuffer;
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

    static Util::Memory::String getBootloaderName();

    static FrameBufferInfo getFrameBufferInfo();

    static Util::Data::Array<MemoryMapEntry> getMemoryMap();

    static const MemoryBlock * getBlockMap();

    static bool hasKernelOption(const Util::Memory::String &key);

    static Util::Memory::String getKernelOption(const Util::Memory::String &key);

    static bool isModuleLoaded(const Util::Memory::String &module);

    static ModuleInfo getModule(const Util::Memory::String &module);

    // Used during the bootstrap process

    static void copyMultibootInfo(const Info *source, uint8_t *destination, uint32_t maxBytes);

    static void readMemoryMap(const Info *multibootInfo);

private:

    static const CopyInformation *copyInformation;
    static const Info *info;

    static const MemoryBlock blockMap[256];
};

}



#endif
