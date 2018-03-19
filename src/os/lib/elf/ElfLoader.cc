#include "ElfLoader.h"

KernelSymbols *ElfLoader::linkerService = nullptr;

extern "C" {
    int resolveSymbol(Elf &elf, uint32_t index);
}

int resolveSymbol(Elf &elf, uint32_t index) {
    return elf.resolveSymbol(index);
}

bool ElfLoader::load(File *file, int argc, char *argv[]) {

    Elf &elf = Elf::from(file, 0x800000);

    if (!elf.isValid()) {
        return false;
    }

    void (*elfProgram)(int, char*[]) = (void(*)(int, char*[])) elf.getEntryPoint();

    elfProgram(argc, argv);

    return true;
}

uint32_t ElfLoader::getSymbol(const String &name) {
    return KernelSymbols::get(name);
}
