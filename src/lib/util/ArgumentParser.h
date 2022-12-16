/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_ARGUMENTPARSER_H
#define HHUOS_ARGUMENTPARSER_H

#include <cstdint>

#include "lib/util/memory/String.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/HashMap.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

namespace Util {

class ArgumentParser {

public:
    /**
     * Default Constructor.
     */
    ArgumentParser() = default;

    /**
     * Copy Constructor.
     */
    ArgumentParser(const ArgumentParser &other) = delete;

    /**
     * Assignment operator.
     */
    ArgumentParser &operator=(const ArgumentParser &other) = delete;

    /**
     * Destructor.
     */
    ~ArgumentParser() = default;

    void setHelpText(const Memory::String &text);

    void addArgument(const Memory::String &name, bool required = false, const Memory::String &abbreviation = "");

    void addSwitch(const Memory::String &name, const Memory::String &abbreviation = "");

    const Memory::String& getErrorString();

    bool parse(uint32_t argc, char *argv[]);

    Data::Array<Memory::String> getUnnamedArguments();

    bool hasArgument(const Memory::String &name);

    Memory::String getArgument(const Memory::String &name);

    bool checkSwitch(const Memory::String &name);

private:

    Memory::String helpText;
    Memory::String errorString;

    Data::ArrayList<Memory::String> parameters;
    Data::ArrayList<Memory::String> switches;
    Data::ArrayList<Memory::String> requiredParameters;
    Data::HashMap<Memory::String, Memory::String> abbreviationMap;

    Data::HashMap<Memory::String, Memory::String> namedArguments;
    Data::ArrayList<Memory::String> parsedSwitches;
    Data::ArrayList<Memory::String> unnamedArguments;
};

}

#endif
