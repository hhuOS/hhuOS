#ifndef __KernelSymbols_include__
#define __KernelSymbols_include__


#include "lib/File.h"
#include "kernel/KernelService.h"

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
     */
    static void initialize();

    /**
     * Inidicates if all kernel symbols have been loaded.
     *
     * @return true, if all kernel symbols have been loaded, false else
     */
    static bool isInitialized();

private:

    static File *symbolFile;

    static File *debugFile;

    static HashMap<String, uint32_t> symbolTable;

    static HashMap<uint32_t, String> debugTable;

    static bool initialized;

    static constexpr char* SYMBOL_FILE_PATH = "/symbols";

    static constexpr char* DEBUG_FILE_PATH = "/debugSymbols";

    static void parseSymbolFile();

};


#endif
