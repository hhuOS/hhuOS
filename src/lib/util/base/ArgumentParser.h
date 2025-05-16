/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include <stdint.h>

#include "lib/util/base/String.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/collection/Array.h"

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

    void setHelpText(const String &text);

    void addArgument(const String &name, bool required = false, const String &abbreviation = "");

    void addSwitch(const String &name, const String &abbreviation = "");

    [[nodiscard]] const String& getErrorString() const;

    [[nodiscard]] bool parse(uint32_t argc, char *argv[]);

    [[nodiscard]] Array<String> getUnnamedArguments() const;

    [[nodiscard]] bool hasArgument(const String &name) const;

    [[nodiscard]] String getArgument(const String &name) const;

    [[nodiscard]] bool checkSwitch(const String &name) const;

private:

    String helpText;
    String errorString;

    ArrayList<String> parameters;
    ArrayList<String> switches;
    ArrayList<String> requiredParameters;
    HashMap<String, String> abbreviationMap;

    HashMap<String, String> namedArguments;
    ArrayList<String> parsedSwitches;
    ArrayList<String> unnamedArguments;
};

}

#endif
