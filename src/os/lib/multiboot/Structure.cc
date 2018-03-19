#include <kernel/memory/MemLayout.h>
#include "Structure.h"

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

    memoryMap = (MemoryMapEntry*) (info.mmap_addr + KERNEL_START);
}

uint32_t Multiboot::Structure::getTotalMem() {

    MemoryMapEntry *entry = nullptr;

    for (uint32_t i = 0; i < info.mmap_length; i++) {

        entry = &memoryMap[i];

        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            continue;
        }

        if (entry->len > 1024 * 1024 * 16) {

            return (uint32_t) entry->len;
        }
    }
}



