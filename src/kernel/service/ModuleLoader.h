/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __ModuleLoader_include__
#define __ModuleLoader_include__

#include "lib/file/File.h"
#include "kernel/module/Module.h"
#include "KernelService.h"
#include "kernel/core/Symbols.h"

namespace Kernel {

/**
 * Loads Modules into the kernel and initializes them.
 *
 * @author Filip Krakowski
 */
class ModuleLoader : public KernelService {

public:

    enum class Status : uint32_t {
        OK = 0x00,
        MISSING_DEP = 0x01,
        INVALID = 0x02,
        WRONG_TYPE = 0x03,
        ERROR = 0x04
    };

    ModuleLoader() = default;

    ModuleLoader(const ModuleLoader &other) = delete;

    ModuleLoader &operator=(const ModuleLoader &other) = delete;

    /**
     * Loads a new module into memory and initializes it.
     *
     * @param file The module's file
     * @return A Status indicating if the operation succeeded
     */
    Status load(File *file);

    static constexpr const char *SERVICE_NAME = "ModuleLoader";

private:

    Util::HashMap<String, Module *> modules;

    static constexpr const char *PROVIDER_SYMBOL = "__provider";

};

}

#endif
