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
