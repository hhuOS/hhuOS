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
    static String get(uint32_t eip);

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

    static Util::HashMap<char*, Address> symbolTable;

    static Util::HashMap<Address, char*> debugTable;

    static bool initialized;

    static void load(const ElfConstants::SectionHeader &sectionHeader);

};


#endif
