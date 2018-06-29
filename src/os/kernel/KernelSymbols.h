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

#ifndef __KernelSymbols_include__
#define __KernelSymbols_include__

#include "lib/Address.h"
#include "lib/String.h"
#include "lib/util/HashMap.h"
#include "lib/multiboot/Constants.h"
#include "lib/elf/ElfConstants.h"

#define KERNEL_LOAD_SYMBOLS 0

/**
 * @author Filip Krakowski
 */
class KernelSymbols {

public:

    KernelSymbols() = delete;

    KernelSymbols(const KernelSymbols &other) = delete;

    /**
     * Searches the memory address for a given symbol.
     *
     * @param name The symbol
     * @return The symbol's address
     */
    static uint32_t get(const String &name);

    /**
     * Returns the symbol name for a given address.
     *
     * @param eip The address
     * @return The symbols name
     */
    static const char* get(uint32_t eip);

    /**
     * Loads all kernel symbols and saves them inside a Map.
     *
     * @param elfInfo The Multiboot ELF information
     */
    static void initialize(const Multiboot::ElfInfo &elfInfo);

    /**
     * Inidicates if all kernel symbols have been loaded.
     *
     * @return true, if all kernel symbols have been loaded, false else
     */
    static bool isInitialized();

private:

    static Multiboot::ElfInfo symbolInfo;

    static Util::HashMap<String, Address> symbolTable;

    static Util::HashMap<Address, char*> debugTable;

    static bool initialized;

    static void load(const ElfConstants::SectionHeader &sectionHeader);

    static const constexpr char *NO_INFORMATION = "<no information>";

};


#endif
