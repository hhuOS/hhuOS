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

#ifndef __Module_include__
#define __Module_include__

#include <cstdint>

#include "lib/string/String.h"

#define MODULE_PROVIDER             \
    extern "C" {                    \
        Module *__provider();       \
    };                              \
    Module *__provider()            \

namespace Kernel {

/**
 * @author Filip Krakowski
 */
class Module {

public:

    Module();

    Module(const Module &other) = delete;

    Module &operator=(const Module &other) = delete;

    virtual ~Module();

    /**
     * Initializes this Module.
     *
     * @return A status code
     */
    virtual int32_t initialize() = 0;

    /**
     * Finalizes this Module.
     *
     * @return A status code
     */
    virtual int32_t finalize() = 0;

    /**
     * Returns this Module's name.
     *
     * @return The Module's name
     */
    virtual String getName() = 0;

    /**
     * Returns the names of all Module's this Module depends on.
     *
     * @return An Array containing all dependencies
     */
    virtual Util::Array<String> getDependencies() = 0;
};

}

#endif
