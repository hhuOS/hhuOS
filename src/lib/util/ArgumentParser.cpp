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

#include "ArgumentParser.h"

namespace Util {

void ArgumentParser::setHelpText(const Memory::String &text) {
    helpText = text;
}

void ArgumentParser::addArgument(const Memory::String &name, bool required, const Memory::String &abbreviation) {
    parameters.add(name);
    abbreviationMap.put(abbreviation, name);
    if (required) {
        requiredParameters.add(name);
    }
}

void ArgumentParser::addSwitch(const Memory::String &name, const Memory::String &abbreviation) {
    switches.add(name);
    abbreviationMap.put(abbreviation, name);
}

const Memory::String& ArgumentParser::getErrorString() {
    return errorString;
}

bool ArgumentParser::parse(uint32_t argc, char *argv[]) {
    namedArguments.clear();
    parsedSwitches.clear();
    unnamedArguments.clear();

    for (uint32_t i = 1; i < argc; i++){
        Memory::String currentArg = argv[i];

        if (!currentArg.beginsWith("-") || currentArg == "-") {
            unnamedArguments.add(currentArg);
        } else if (currentArg == "-h" || currentArg == "--help") {
            errorString = helpText;
            return false;
        } else {
            if (abbreviationMap.containsKey(currentArg.substring(1, currentArg.length()))) {
                currentArg = abbreviationMap.get(currentArg.substring(1, currentArg.length()));
            } else {
                if (currentArg.beginsWith("--")) {
                    currentArg = currentArg.substring(2, currentArg.length());
                } else {
                    currentArg = currentArg.substring(1, currentArg.length());
                }
            }

            if (requiredParameters.contains(currentArg)) {
                requiredParameters.remove(currentArg);
            }

            if (parameters.contains(currentArg)) {
                if (i < argc - 1) {
                    namedArguments.put(currentArg, argv[++i]);
                } else {
                    errorString = "No value given for parameter '" + currentArg + "'!";

                    return false;
                }
            } else if (switches.contains(currentArg)) {
                parsedSwitches.add(currentArg);
            } else {
                errorString = "Unknown parameter '" + currentArg + "'!";

                return false;
            }
        }
    }

    if (!requiredParameters.isEmpty()) {
        errorString = "Missing required parameters: ";

        for (const Memory::String &name : requiredParameters) {
            errorString += "'" + name + "' ";
        }

        return false;
    }

    return true;
}

Data::Array<Memory::String> ArgumentParser::getUnnamedArguments() {
    return unnamedArguments.toArray();
}

bool ArgumentParser::hasArgument(const Memory::String &name) {
    return namedArguments.containsKey(name);
}

Memory::String ArgumentParser::getArgument(const Memory::String &name) {
    if (namedArguments.containsKey(name)) {
        return namedArguments.get(name);
    }

    return "";
}

bool ArgumentParser::checkSwitch(const Memory::String &name) {
    return parsedSwitches.contains(name);
}

}