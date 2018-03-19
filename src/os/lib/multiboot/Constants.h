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

    struct AoutSymbolTable
    {
        uint32_t tabsize;
        uint32_t strsize;
        uint32_t addr;
        uint32_t reserved;
    };

    struct ElfSectionHeaderTable
    {
        uint32_t num;
        uint32_t size;
        uint32_t addr;
        uint32_t shndx;
    };

    struct Info {

        /* Multiboot info version number */
        uint32_t flags;

        /* Available memory from BIOS */
        uint32_t mem_lower;
        uint32_t mem_upper;

        /* "root" partition */
        uint32_t boot_device;

        /* Kernel command line */
        uint32_t cmdline;

        /* Boot-Module list */
        uint32_t mods_count;
        uint32_t mods_addr;

        union {
            AoutSymbolTable aout_sym;
            ElfSectionHeaderTable elf_sec;
        } u;

        /* Memory Mapping buffer */
        uint32_t mmap_length;
        uint32_t mmap_addr;

        /* Drive Info buffer */
        uint32_t drives_length;
        uint32_t drives_addr;

        /* ROM configuration table */
        uint32_t config_table;

        /* Boot Loader Name */
        uint32_t boot_loader_name;

        /* APM table */
        uint32_t apm_table;

        /* Video */
        uint32_t vbe_control_info;
        uint32_t vbe_mode_info;
        uint16_t vbe_mode;
        uint16_t vbe_interface_seg;
        uint16_t vbe_interface_off;
        uint16_t vbe_interface_len;

        uint64_t framebuffer_addr;
        uint32_t framebuffer_pitch;
        uint32_t framebuffer_width;
        uint32_t framebuffer_height;
        uint8_t framebuffer_bpp;
        uint8_t framebuffer_type;

        union {

            struct
            {
                uint32_t framebuffer_palette_addr;
                uint16_t framebuffer_palette_num_colors;
            };

            struct
            {
                uint8_t framebuffer_red_field_position;
                uint8_t framebuffer_red_mask_size;
                uint8_t framebuffer_green_field_position;
                uint8_t framebuffer_green_mask_size;
                uint8_t framebuffer_blue_field_position;
                uint8_t framebuffer_blue_mask_size;
            };
        };
    };

    struct MemoryMapEntry {
        uint32_t size;
        uint64_t addr;
        uint64_t len;
        uint32_t type;
    } __attribute__((packed));

}


#endif
