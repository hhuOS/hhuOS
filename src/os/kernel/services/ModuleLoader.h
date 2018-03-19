#ifndef __ModuleLoader_include__
#define __ModuleLoader_include__


#include <kernel/Module.h>
#include "kernel/KernelService.h"
#include "kernel/KernelSymbols.h"

class ModuleLoader : public KernelService {

public:

    enum class Status : uint32_t {
        OK              = 0x00,
        MISSING_DEP     = 0x01,
        INVALID         = 0x02,
        WRONG_TYPE      = 0x03,
        ERROR           = 0x04
    };

    ModuleLoader() = default;

    ModuleLoader(const ModuleLoader &other) = delete;

    Status load(File *file);

    static constexpr const char* SERVICE_NAME = "ModuleLoader";

private:

    HashMap<String, Module*> modules;

};


#endif
