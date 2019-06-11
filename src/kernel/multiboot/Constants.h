/*
 * Copyright (C) 1999,2003,2007,2008,2009,2010  Free Software Foundation, Inc.
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

#ifndef __Constants_include__
#define __Constants_include__

#include <cstdint>

namespace Kernel::Multiboot {

static const uint32_t FRAMEBUFFER_TYPE_INDEXED = 0;
static const uint32_t FRAMEBUFFER_TYPE_RGB = 1;
static const uint32_t FRAMEBUFFER_TYPE_EGA_TEXT = 2;

static const uint32_t MULTIBOOT_INFO_MEMORY = 0x00000001;
static const uint32_t MULTIBOOT_INFO_BOOTDEV = 0x00000002;
static const uint32_t MULTIBOOT_INFO_CMDLINE = 0x00000004;
static const uint32_t MULTIBOOT_INFO_MODS = 0x00000008;
static const uint32_t MULTIBOOT_INFO_AOUT_SYMS = 0x00000010;
static const uint32_t MULTIBOOT_INFO_ELF_SHDR = 0x00000020;
static const uint32_t MULTIBOOT_INFO_MEM_MAP = 0x00000040;
static const uint32_t MULTIBOOT_INFO_DRIVE_INFO = 0x00000080;
static const uint32_t MULTIBOOT_INFO_CONFIG_TABLE = 0x00000100;
static const uint32_t MULTIBOOT_INFO_BOOT_LOADER_NAME = 0x00000200;
static const uint32_t MULTIBOOT_INFO_APM_TABLE = 0x00000400;
static const uint32_t MULTIBOOT_INFO_VBE_INFO = 0x00000800;
static const uint32_t MULTIBOOT_INFO_FRAMEBUFFER_INFO = 0x00001000;

static const uint32_t MULTIBOOT_MEMORY_AVAILABLE = 1;
static const uint32_t MULTIBOOT_MEMORY_RESERVED = 2;
static const uint32_t MULTIBOOT_MEMORY_ACPI_RECLAIMABLE = 3;
static const uint32_t MULTIBOOT_MEMORY_NVS = 4;
static const uint32_t MULTIBOOT_MEMORY_BADRAM = 5;

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
    char *string;
    uint32_t reserved;

    bool operator!=(const ModuleInfo &other) {

        return start != other.start;
    }
};

struct FrameBufferInfo {
    void *address;
    uint16_t width;
    uint16_t height;
    uint16_t pitch;
    uint8_t bpp;
    uint8_t type;
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

    uint64_t framebufferAddress;
    uint32_t framebufferPitch;
    uint32_t framebufferWidth;
    uint32_t framebufferHeight;
    uint8_t framebufferBpp;
    uint8_t framebufferType;

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

struct MemoryMapEntry {
    uint32_t size;
    uint64_t address;
    uint64_t length;
    uint32_t type;

    bool operator!=(const MemoryMapEntry &other) const {

        return address != other.address;
    }
};

}

#endif
