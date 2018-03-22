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

#ifndef __Module_include__
#define __Module_include__

#include "lib/deprecated/HashMap.h"
#include "lib/elf/Elf.h"
#include "lib/String.h"

/**
 * @author Filip Krakowski
 */
class Module {

public:

    Module() = default;

    Module(const Module &other) = delete;

    ~Module();

    /**
     * Calls this Module's init() function.
     *
     * @return The value returned by init()
     */
    int initialize();

    /**
     * Calls this Module's fini() function.
     *
     * @return  The value returned by fini()
     */
    int finalize();

    /**
     * Returns the address for a given symbol.
     *
     * @param name The symbol's name
     * @return The symbol's address
     */
    uint32_t getSymbol(const String &name);

    /**
     * Indicates if this Module has a valid header.
     *
     * @return true, if the Module's header is valid, false else
     */
    bool isValid();

private:

    uint32_t base;

    int (*init)() = nullptr;

    int (*fini)() = nullptr;

    char *buffer = nullptr;

    FileHeader *fileHeader;

    String name;

    Util::HashMap<String, uint32_t> localSymbols;

    Util::HashMap<String, SectionHeader*> sections;

    SymbolEntry *symbolTable = nullptr;

    uint32_t symbolTableSize;

    char *stringTable = nullptr;

    uint32_t stringTableSize;

    char *sectionNames;

    char *getSectionName(uint16_t sectionIndex);

    void loadSectionNames();

    void loadSections();

    void parseSymbolTable();

    void relocate();

    friend class ModuleLoader;

};


#endif
