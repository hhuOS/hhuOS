#include <kernel/memory/MemLayout.h>
#include <kernel/KernelSymbols.h>
#include "Structure.h"
#include "Constants.h"

extern "C" {
    void parse_multiboot(Multiboot::Info *address);
}

void parse_multiboot(Multiboot::Info *address) {

    Multiboot::Structure::parse(address);
}


Multiboot::Info Multiboot::Structure::info;

Multiboot::MemoryMapEntry *Multiboot::Structure::memoryMap;

void Multiboot::Structure::parse(Multiboot::Info *address) {

    info = *address;

    memoryMap = (MemoryMapEntry*) (info.memoryMapAddress + KERNEL_START);

    if (info.flags & MULTIBOOT_INFO_ELF_SHDR) {

        info.symbols.elf.address += KERNEL_START;

        KernelSymbols::initialize(info.symbols.elf);
    }
}

uint32_t Multiboot::Structure::getTotalMem() {

    MemoryMapEntry *entry = nullptr;

    for (uint32_t i = 0; i < info.memoryMapLength; i++) {

        entry = &memoryMap[i];

        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            continue;
        }

        if (entry->length > 1024 * 1024 * 16) {

            return (uint32_t) entry->length;
        }
    }
}



