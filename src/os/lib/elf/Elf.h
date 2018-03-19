#ifndef __Elf_include__
#define __Elf_include__

#include <cstdint>
#include <lib/String.h>
#include <lib/File.h>
#include "lib/elf/ElfConstants.h"

using namespace ElfConstants;

/**
 * @author Filip Krakowski
 */
class Elf {

public:

    Elf(const Elf &other) = delete;

    static Elf& from(File *elfFile, uint32_t base);

    bool isValid();

    uint32_t getEntryPoint();

    uint32_t resolveSymbol(uint32_t index);

    SectionHeader getSectionHeader(const String &name);

private:

    explicit Elf(File *elfFile, uint32_t base);

    uint32_t base;

    FileHeader fileHeader;

    SymbolEntry *dynamicSymbolTable = nullptr;

    DynamicEntry *dynamic = nullptr;

    File *elfFile = nullptr;

    HashMap<String, SectionHeader> sectionHeaders;

    HashMap<String, uint32_t> symbolTable;

    uint32_t *globalOffsetTable = nullptr;

    uint32_t *hashTable = nullptr;

    char *sectionNames = nullptr;

    char *dynamicStringTable = nullptr;

    uint32_t dynamicStringTableSize;

    uint32_t dynamicSymbolTableEntries;

    uint32_t relocationTableEntries;

    void readFileHeader();

    void readProgramHeaders();

    void processProgramHeader(const ProgramHeader &programHeader);

    void parseDynamicTable();

    void loadDynamicSymbols();

    void readSectionHeaders();

    void readSectionNames();

    void adjustGlobalOffsetTable();

    void relocate();

    bool valid = true;

};


#endif
