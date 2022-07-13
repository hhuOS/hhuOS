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

#ifndef __KernelSymbols_include__
#define __KernelSymbols_include__

#include "lib/util/memory/Address.h"
#include "lib/util/memory/String.h"
#include "lib/util/data/HashMap.h"
#include "kernel/multiboot/Constants.h"
#include "lib/util/file/elf/Constants.h"

namespace Kernel {

/**
 * @author Filip Krakowski
 */
class Symbols {

public:

    Symbols() = delete;

    Symbols(const Symbols &other) = delete;

    /**
     * Searches the memory address for a given symbol.
     *
     * @param name The symbol
     * @return The symbol's address
     */
    static uint32_t get(const Util::Memory::String &name);

    /**
     * Returns the symbol name for a given address.
     *
     * @param eip The address
     * @return The symbols name
     */
    static const char *get(uint32_t eip);

    /**
     * Loads all kernel symbols and saves them inside a Map.
     *
     * @param elfInfo The Multiboot ELF information
     */
    static void initialize(const Multiboot::ElfInfo &elfInfo);

    /**
     * Indicates if all kernel symbols have been loaded.
     *
     * @return true, if all kernel symbols have been loaded, false else
     */
    static bool isInitialized();
    
    /**
     * Copy the symbols to memory. (Call before paging is enabled.)
     */
    static Util::Memory::Address<uint32_t> copy(const Multiboot::ElfInfo &elfInfo, Util::Memory::Address<uint32_t> destination);

private:

    static Multiboot::ElfInfo symbolInfo;

    static Util::Data::HashMap<Util::Memory::String, Util::Memory::Address<uint32_t>> symbolTable;

    static Util::Data::HashMap<Util::Memory::Address<uint32_t>, char *> debugTable;

    static bool initialized;

    static void load(const Util::File::Elf::Constants::SectionHeader &sectionHeader);

    static const constexpr char *NO_INFORMATION = "<no information>";

};

}

#endif
