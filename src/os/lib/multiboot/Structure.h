#ifndef __Multiboot_include__
#define __Multiboot_include__

#include <cstdint>
#include "Constants.h"

namespace Multiboot {

    class Structure {

    public:

        Structure() = delete;

        Structure(const Structure &other) = delete;

        Structure &operator=(const Structure &other) = delete;

        ~Structure() = delete;

        static void parse(Info *address);

        static uint32_t getTotalMem();

    private:

        static Info info;

        static MemoryMapEntry *memoryMap;

    };
}



#endif
