#ifndef __ElfLoader_include__
#define __ElfLoader_include__

#include <stdint.h>
#include <lib/File.h>
#include <kernel/KernelSymbols.h>
#include "Elf.h"


class ElfLoader {

public:

    static bool load(File *file, int argc = 0, char *argv[] = nullptr);

    static uint32_t getSymbol(const String &name);

private:

    static KernelSymbols *linkerService;

};


#endif
