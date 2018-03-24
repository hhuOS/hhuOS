/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __Elf_include__
#define __Elf_include__

#include <cstdint>
#include <lib/String.h>
#include <lib/file/File.h>
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

    Util::HashMap<String, SectionHeader> sectionHeaders;

    Util::HashMap<String, uint32_t> symbolTable;

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
