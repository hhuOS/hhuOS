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

#ifndef HHUOS_ELFFILE_H
#define HHUOS_ELFFILE_H

#include <cstdint>

namespace Util::File::Elf {

enum class ElfType : uint16_t {
    NONE = 0x00,
    RELOCATABLE = 0x01,
    EXECUTABLE = 0x02,
    DYNAMIC = 0x03
};

enum class SectionHeaderType : uint32_t {
    NONE = 0x00,
    PROGBITS = 0x01,
    SYMTAB = 0x02,
    STRTAB = 0x03,
    RELA = 0x04,
    HASH = 0x05,
    DYNAMIC = 0x06,
    NOTE = 0x07,
    NOBITS = 0x08,
    REL = 0x09,
    SHLIB = 0x0A,
    DYNSYM = 0x0B,
    INIT_ARRAY = 0x0E,
    FINI_ARRAY = 0x0F,
    PREINIT_ARRAY = 0x10,
    GROUP = 0x11,
    SYMTAB_SHNDX = 0x12,
    NUM = 0x13
};

enum class ProgramHeaderType : uint32_t {
    NONE = 0x00,
    LOAD = 0x01,
    DYNAMIC = 0x02,
    INTERP = 0x03,
    NOTE = 0x04,
    SHLIB = 0x05,
    PHDR = 0x06,
};

enum class MachineType : uint16_t {
    X86 = 0x03
};

enum class Architecture : uint8_t {
    BIT_32 = 0x01
};

enum class ByteOrder : uint8_t {
    LITTLE_END = 0x01
};

enum class RelocationType : uint8_t {
    R_386_NONE = 0x00,
    R_386_32 = 0x01,
    R_386_PC32 = 0x02,
    R_386_GOT32 = 0x03,
    R_386_PLT32 = 0x04,
    R_386_COPY = 0x05,
    R_386_GLOB_DAT = 0x06,
    R_386_JMP_SLOT = 0x07,
    R_386_RELATIVE = 0x08,
    R_386_GOTOFF = 0x09,
    R_386_GOTPC = 0x0A,
    R_386_32PLT = 0x0B,
};

enum class DynamicTag : uint32_t {
    NONE = 0x00,
    NEEDED = 0x01,
    PLTRELSZ = 0x02,
    PLTGOT = 0x03,
    HASH = 0x04,
    STRTAB = 0x05,
    SYMTAB = 0x06,
    RELA = 0x07,
    RELASZ = 0x08,
    RELAENT = 0x09,
    STRSZ = 0x0A,
    SYMENT = 0x0B,
    INIT = 0x0C,
    FINI = 0x0D,
    SONAME = 0x0E,
    RPATH = 0x0F,
    SYMBOLIC = 0x10,
    REL = 0x11,
    RELSZ = 0x12,
    RELENT = 0x13,
    PLTREL = 0x14,
    DEBUG = 0x15,
    TEXTREL = 0x16,
    JMPREL = 0x17
};

enum class SymbolType : uint8_t {
    NOTYPE = 0x00,
    OBJECT = 0x01,
    FUNC = 0x02,
    SECTION = 0x03,
    FILE = 0x04,
    LOPROC = 0x0D,
    HIPROC = 0x0F
};

enum class SymbolBinding : uint8_t {
    LOCAL = 0x00,
    GLOBAL = 0x01,
    WEAK = 0x02,
    LOPROC = 0x0D,
    HIPROC = 0x0F
};

struct FileHeader {
    uint8_t magic[4];
    Architecture architecture;
    ByteOrder byteOrder;
    uint8_t elfVersion;
    uint8_t osAbi;
    uint8_t abiVersion;
    uint8_t padding[7];

    ElfType type;
    MachineType machine;
    uint32_t version;
    uint32_t entry;
    uint32_t programHeader;
    uint32_t sectionHeader;
    uint32_t flags;
    uint16_t headerSize;
    uint16_t programHeaderEntrySize;
    uint16_t programHeaderEntries;
    uint16_t sectionHeaderEntrySize;
    uint16_t sectionHeaderEntries;
    uint16_t sectionHeaderStringIndex;

    bool isValid();

    bool hasProgramEntries() const;

} __attribute__((packed));

struct SectionHeader {
    uint32_t nameOffset;
    SectionHeaderType type;
    uint32_t flags;
    uint32_t virtualAddress;
    uint32_t offset;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t alignment;
    uint32_t entrySize;
} __attribute__((packed));

struct ProgramHeader {
    ProgramHeaderType type;
    uint32_t offset;
    uint32_t virtualAddress;
    uint32_t physicalAddress;
    uint32_t fileSize;
    uint32_t memorySize;
    uint32_t flags;
    uint32_t alignment;
} __attribute__((packed));

struct SymbolEntry {
    uint32_t nameOffset;
    uint32_t value;
    uint32_t size;
    uint8_t info;
    uint8_t other;
    uint16_t section;

    [[nodiscard]] SymbolBinding getSymbolBinding() const;
    [[nodiscard]] SymbolType getSymbolType() const;
} __attribute__((packed));

struct RelocationEntry {
    uint32_t offset;
    uint32_t info;

    [[nodiscard]] uint32_t getSymbolIndex() const;
    [[nodiscard]] RelocationType getType() const;
} __attribute__((packed));

struct DynamicEntry {
    DynamicTag tag;
    uint32_t value;
} __attribute__((packed));

class File {

public:

    explicit File(uint8_t *buffer);

    File(const File &other) = delete;

    File& operator=(const File &other) = delete;

    ~File();

    uint32_t getEndAddress();

    void loadProgram();

    [[nodiscard]] int32_t (*getEntryPoint() const)(int, char**) {
        return reinterpret_cast<int (*)(int, char**)>(fileHeader.entry);
    }

private:

    uint8_t *buffer;
    FileHeader &fileHeader;

    char *sectionNames = nullptr;
    ProgramHeader *programHeaders = nullptr;
    SectionHeader *sectionHeaders = nullptr;

};

static constexpr const char* SECTION_NAME_INTERP = ".interp";
static constexpr const char* SECTION_NAME_DYNSYM = ".dynsym";
static constexpr const char* SECTION_NAME_DYNSTR = ".dynstr";
static constexpr const char* SECTION_NAME_REL = ".rel.plt";
static constexpr const char* SECTION_NAME_PLT = ".plt";
static constexpr const char* SECTION_NAME_TEXT = ".text";
static constexpr const char* SECTION_NAME_RODATA = ".rodata";
static constexpr const char* SECTION_NAME_DYNAMIC = ".dynamic";
static constexpr const char* SECTION_NAME_GOT = ".got.plt";
static constexpr const char* SECTION_NAME_COMMENT = ".comment";
static constexpr const char* SECTION_NAME_SHSTRTAB = ".shstrtab";
static constexpr const char* SECTION_NAME_SYMTAB = ".symtab";
static constexpr const char* SECTION_NAME_STRTAB = ".strtab";

}

#endif
