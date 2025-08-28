/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_LIB_UTIL_IO_ELFFILE_H
#define HHUOS_LIB_UTIL_IO_ELFFILE_H

#include <stddef.h>
#include <stdint.h>

#include "File.h"

namespace Util::Io {

/// Class to parse ELF files and load the contained program sections into memory.
/// This implementation only supports 32-bit x86 ELF files.
/// It does not support dynamic linking or relocation (yet).
class ElfFile {

public:
    /// Different types of ELF sections.
    enum class SectionType : uint32_t {
        /// Section table entry is unused.
        NONE = 0x00,
        /// Section contains program data (e.g. code, initialized data).
        PROGBITS = 0x01,
        /// Section contains a symbol table.
        SYMTAB = 0x02,
        /// Section contains a string table.
        STRTAB = 0x03,
        /// Section contains relocation entries with explicit addends.
        RELA = 0x04,
        /// Section contains a symbol hash table.
        HASH = 0x05,
        /// Section contains dynamic linking information.
        DYNAMIC = 0x06,
        /// Section contains notes.
        NOTE = 0x07,
        /// Section contains uninitialized program data.
        NOBITS = 0x08,
        /// Section contains relocation entries without explicit addends.
        REL = 0x09,
        /// Section contains a dynamic linker symbol table.
        DYNSYM = 0x0b,
        /// Section contains an array of constructors.
        INIT_ARRAY = 0x0e,
        /// Section contains an array of destructors.
        FINI_ARRAY = 0x0f,
        /// Section contains an array of pre-constructors.
        PREINIT_ARRAY = 0x10,
        /// Section defines a section group.
        GROUP = 0x11,
        /// Section contains extended section indices.
        SYMTAB_SHNDX = 0x12
    };

    /// The header that prepends each section in the ELF file.
    struct SectionHeader {
        /// Offset into the string table where the name of this section is stored as a null-terminated string.
        uint32_t nameOffset;
        /// Type of this section.
        SectionType type;
        /// Flags for this section.
        uint32_t flags;
        /// Address where this section should be loaded into memory.
        uint32_t virtualAddress;
        /// Offset of this section in the ELF file.
        uint32_t offset;
        /// Size of this section in bytes.
        uint32_t size;
        /// Section index of an associated section.
        uint32_t link;
        /// Additional section information.
        uint32_t info;
        /// Alignment constraint for this section.
        uint32_t alignment;
        /// If the section contains a table (e.g. symbol table), size of each entry in bytes.
        uint32_t entrySize;
    } __attribute__((packed));

    /// Different types of symbols in the symbol table.
    enum class SymbolType : uint8_t {
        /// No type specified (e.g. undefined symbol).
        NO_TYPE = 0x00,
        /// Symbol is a data object (e.g. variable, array).
        OBJECT = 0x01,
        /// Symbol is a function or executable code.
        FUNC = 0x02,
        /// Symbol is a section.
        SECTION = 0x03,
        /// Symbol is a file name.
        FILE = 0x04,
        /// Start of a range of processor-specific symbol types.
        LOPROC = 0x0d,
        /// End of a range of processor-specific symbol types.
        HIPROC = 0x0f
    };

    /// Different types symbol bindings in the symbol table.
    enum class SymbolBinding : uint8_t {
        /// Local symbol, not visible outside the object file containing its definition.
        LOCAL = 0x00,
        /// Global symbol, visible to all object files being combined.
        GLOBAL = 0x01,
        /// Weak symbol, like global but with lower precedence (may be overridden).
        WEAK = 0x02,
        /// Start of a range of processor-specific symbol bindings.
        LOPROC = 0x0d,
        /// End of a range of processor-specific symbol bindings.
        HIPROC = 0x0f
    };

    /// Description of an entry in the symbol table.
    struct SymbolEntry {
        /// Offset into the string table where the name of this symbol is stored as a null-terminated string.
        uint32_t nameOffset;
        /// Value of the symbol (e.g. address).
        uint32_t value;
        /// Size of the symbol (e.g. size of variable or function in bytes).
        uint32_t size;
        /// Symbol type and binding attributes (Use `getSymbolType()` and `getSymbolBinding()` to extract).
        uint8_t info;
        /// Symbol visibility (e.g. hidden, protected).
        uint8_t other;
        /// Section index this symbol is defined in (e.g. text, data) or special values.
        uint16_t section;

        /// Extract the symbol binding from the `info` field.
        [[nodiscard]] SymbolBinding getSymbolBinding() const;

        /// Extract the symbol type from the `info` field.
        [[nodiscard]] SymbolType getSymbolType() const;
    } __attribute__((packed));

    /// Create an elf file instance from a buffer containing the ELF file data.
    /// The elf file instance will not take ownership of the memory and will not free it on destruction.
    explicit ElfFile(uint8_t *buffer);

    /// Create an elf file instance from a File object.
    /// The whole file is read into memory for parsing.
    explicit ElfFile(const File &file);

    /// ElfFile is not copyable, since it manages a memory buffer.
    ElfFile(const ElfFile &other) = delete;

    /// ElfFile is not copyable, since it manages a memory buffer.
    ElfFile& operator=(const ElfFile &other) = delete;

    /// Destroy the elf file instance and free any allocated memory.
    ~ElfFile();

    /// Load all program sections marked for loading into memory at their specified virtual addresses.
    /// CAUTION: This will overwrite any existing data at the target addresses!
    ///          It is only intended to load an executable program into a fresh virtual address space.
    void loadProgram() const;

    /// Calculate the highest virtual address used by any loadable program section.
    [[nodiscard]] uintptr_t getEndAddress() const;

    /// Get the entry point of the ELF file, i.e. the virtual address where execution should start.
    /// This is typically the address of the `_start` function.
    [[nodiscard]] int (*getEntryPoint() const)(int, char**);

    /// Get the section header of a specific type.
    /// If multiple sections of the same type exist, the first one is returned.
    /// If no such section exists, a panic is fired.
    [[nodiscard]] const SectionHeader& getSectionHeader(SectionType headerType) const;

private:

    enum class ElfType : uint16_t {
        NONE = 0x00,
        RELOCATABLE = 0x01,
        EXECUTABLE = 0x02,
        DYNAMIC = 0x03
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

        [[nodiscard]] bool isValid() const;

        [[nodiscard]] bool hasProgramEntries() const;

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

    struct RelocationEntry {
        uint32_t offset;
        uint32_t info;

        [[nodiscard]] size_t getSymbolIndex() const;
        [[nodiscard]] RelocationType getType() const;
    } __attribute__((packed));

    struct DynamicEntry {
        DynamicTag tag;
        uint32_t value;
    } __attribute__((packed));

    void parseFileHeader();

    bool deleteBuffer;
    uint8_t *buffer;
    const FileHeader &fileHeader = *reinterpret_cast<FileHeader*>(buffer);

    char *sectionNames = nullptr;
    ProgramHeader *programHeaders = nullptr;
    SectionHeader *sectionHeaders = nullptr;

    static constexpr auto SECTION_NAME_INTERP = ".interp";
    static constexpr auto SECTION_NAME_DYNSYM = ".dynsym";
    static constexpr auto SECTION_NAME_DYNSTR = ".dynstr";
    static constexpr auto SECTION_NAME_REL = ".rel.plt";
    static constexpr auto SECTION_NAME_PLT = ".plt";
    static constexpr auto SECTION_NAME_TEXT = ".text";
    static constexpr auto SECTION_NAME_RODATA = ".rodata";
    static constexpr auto SECTION_NAME_DYNAMIC = ".dynamic";
    static constexpr auto SECTION_NAME_GOT = ".got.plt";
    static constexpr auto SECTION_NAME_COMMENT = ".comment";
    static constexpr auto SECTION_NAME_SHSTRTAB = ".shstrtab";
    static constexpr auto SECTION_NAME_SYMTAB = ".symtab";
    static constexpr auto SECTION_NAME_STRTAB = ".strtab";
};

}

#endif
