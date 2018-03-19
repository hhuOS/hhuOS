#ifndef __Constants_include__
#define __Constants_include__

#include <cstdint>

namespace Multiboot {

    static const uint32_t FRAMEBUFFER_TYPE_INDEXED                = 0;
    static const uint32_t FRAMEBUFFER_TYPE_RGB                    = 1;
    static const uint32_t FRAMEBUFFER_TYPE_EGA_TEXT               = 2;

    static const uint32_t MULTIBOOT_INFO_MEMORY                   = 0x00000001;
    static const uint32_t MULTIBOOT_INFO_BOOTDEV                  = 0x00000002;
    static const uint32_t MULTIBOOT_INFO_CMDLINE                  = 0x00000004;
    static const uint32_t MULTIBOOT_INFO_MODS                     = 0x00000008;
    static const uint32_t MULTIBOOT_INFO_AOUT_SYMS                = 0x00000010;
    static const uint32_t MULTIBOOT_INFO_ELF_SHDR                 = 0x00000020;
    static const uint32_t MULTIBOOT_INFO_MEM_MAP                  = 0x00000040;
    static const uint32_t MULTIBOOT_INFO_DRIVE_INFO               = 0x00000080;
    static const uint32_t MULTIBOOT_INFO_CONFIG_TABLE             = 0x00000100;
    static const uint32_t MULTIBOOT_INFO_BOOT_LOADER_NAME         = 0x00000200;
    static const uint32_t MULTIBOOT_INFO_APM_TABLE                = 0x00000400;
    static const uint32_t MULTIBOOT_INFO_VBE_INFO                 = 0x00000800;
    static const uint32_t MULTIBOOT_INFO_FRAMEBUFFER_INFO         = 0x00001000;

    static const uint32_t  MULTIBOOT_MEMORY_AVAILABLE             = 1;
    static const uint32_t  MULTIBOOT_MEMORY_RESERVED              = 2;
    static const uint32_t  MULTIBOOT_MEMORY_ACPI_RECLAIMABLE      = 3;
    static const uint32_t  MULTIBOOT_MEMORY_NVS                   = 4;
    static const uint32_t  MULTIBOOT_MEMORY_BADRAM                = 5;

    struct AoutInfo
    {
        uint32_t tabSize;
        uint32_t strsize;
        uint32_t address;
        uint32_t reserved;
    };

    struct ElfInfo
    {
        uint32_t sectionCount;
        uint32_t sectionSize;
        uint32_t address;
        uint32_t stringSectionIndex;
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
        uint32_t framebufferHeigth;
        uint8_t  framebufferBpp;
        uint8_t  framebufferType;

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
    } __attribute__((packed));
}


#endif
