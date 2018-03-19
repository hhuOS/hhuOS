#ifndef __KernelSymbols_include__
#define __KernelSymbols_include__


#include "lib/File.h"
#include "kernel/KernelService.h"

#define KERNEL_LOAD_SYMBOLS 0

class KernelSymbols {

public:

    KernelSymbols() = delete;

    KernelSymbols(const KernelSymbols &other) = delete;

    static uint32_t get(const String &name);

    static String get(uint32_t eip);

    static void initialize();

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
