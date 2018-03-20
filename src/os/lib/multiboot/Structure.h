#ifndef __Multiboot_include__
#define __Multiboot_include__

#include <cstdint>
#include "Constants.h"
#include "lib/util/HashMap.h"

namespace Multiboot {

    class Structure {

    public:

        Structure() = delete;

        Structure(const Structure &other) = delete;

        Structure &operator=(const Structure &other) = delete;

        ~Structure() = delete;

        static void parse(Info *address);

        static uint32_t getTotalMem();

        static Multiboot::ModuleInfo getModule(const String &module);

        static bool isModuleLoaded(const String &module);

    private:

        static Info info;

        static MemoryMapEntry *memoryMap;

        static Util::HashMap<String, Multiboot::ModuleInfo> modules;

    };
}



#endif
