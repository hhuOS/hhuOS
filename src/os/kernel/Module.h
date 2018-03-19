#ifndef __Module_include__
#define __Module_include__

#include "lib/deprecated/HashMap.h"
#include "lib/elf/Elf.h"
#include "lib/String.h"

class Module {

public:

    Module() = default;

    Module(const Module &other) = delete;

    ~Module();

    int initialize();

    int finalize();

    uint32_t getSymbol(const String &name);

    bool isValid();

private:

    uint32_t base;

    int (*init)() = nullptr;

    int (*fini)() = nullptr;

    char *buffer = nullptr;

    FileHeader *fileHeader;

    String name;

    HashMap<String, uint32_t> localSymbols;

    HashMap<String, SectionHeader*> sections;

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
