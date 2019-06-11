/*
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

#ifndef __ElfModule_include__
#define __ElfModule_include__

#include "lib/util/HashMap.h"
#include "lib/elf/Elf.h"
#include "lib/string/String.h"
#include "kernel/module/Module.h"
#include "kernel/service/ModuleLoader.h"

/**
 * @author Filip Krakowski
 */
class ElfModule {

public:

    ElfModule() = default;

    ElfModule(const ElfModule &other) = delete;

    ~ElfModule();

    /**
     * Returns an instance of the module's implementation.
     *
     * @return An instance of the module
     */
    Kernel::Module *getInstance();

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

    Kernel::Module* (*provider)() = nullptr;

    Kernel::Module* instance = nullptr;

    char *buffer = nullptr;

    FileHeader *fileHeader;

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

    friend class Kernel::ModuleLoader;

};


#endif
