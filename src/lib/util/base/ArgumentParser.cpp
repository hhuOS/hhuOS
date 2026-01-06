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

#include <stddef.h>

#include "ArgumentParser.h"

namespace Util {

bool ArgumentParser::parse(const uint32_t argc, char *argv[]) {
    ArrayList<String> unnamedArgumentsList;
    namedArguments.clear();
    parsedSwitches.clear();

    for (size_t i = 1; i < argc; i++){
        const String currentArg = argv[i];

        size_t hyphens = 0;
        while (hyphens < currentArg.length() && currentArg[hyphens] == '-') {
            hyphens++;
        }

        if (hyphens == 0 || hyphens > 2 || currentArg.length() == hyphens) {
            unnamedArgumentsList.add(currentArg);
        } else if (currentArg == "-h" || currentArg == "--help") {
            errorString = helpText;
            namedArguments.clear();
            parsedSwitches.clear();
            return false;
        } else {
            String parameter = currentArg;
            if (hyphens == 1) {
                // Short form (abbreviation):
                // Remove leading '-' and search for full parameter name in abbreviation map.
                // If not found, leave parameter untouched (with leading '-').
                const auto abbreviation = currentArg.substring(1);
                if (abbreviationMap.containsKey(abbreviation)) {
                    parameter = abbreviationMap.get(abbreviation);
                }
            } else {
                // Long form:
                // Remove leading '--'.
                parameter = currentArg.substring(2);
            }

            // Check if parameter is required and remove it from the list of required parameters.
            if (requiredParameters.contains(parameter)) {
                requiredParameters.remove(parameter);
            }

            // Check if parameter is known.
            if (parameters.contains(parameter)) {
                // Parameter requires a value -> Get next argument as value.
                if (i < argc - 1) {
                    const String value = argv[i + 1];
                    namedArguments.put(parameter, value);
                } else {
                    // No value given for parameter -> Quit parsing with error.
                    errorString = "No value given for parameter '" + currentArg + "'!";
                    namedArguments.clear();
                    parsedSwitches.clear();
                    return false;
                }
            } else if (switches.contains(parameter)) {
                // Switch found -> This is a boolean flag, so just store its presence.
                parsedSwitches.add(parameter);
            } else if (ignoreUnknownArguments) {
                // The argument looks like a parameter or switch, but is unknown.
                // Since 'ignoreUnknownArguments' is set, treat it as unnamed argument instead of throwing an error.
                unnamedArgumentsList.add(currentArg);
            } else {
                // Unknown parameter -> Quit parsing with error.
                errorString = "Unknown parameter '" + parameter + "'!";
                namedArguments.clear();
                parsedSwitches.clear();
                return false;
            }
        }
    }

    // Check if all required parameters were given.
    if (!requiredParameters.isEmpty()) {
        errorString = "Missing required parameters: ";

        for (const String &name : requiredParameters) {
            errorString += "'" + name + "' ";
        }

        namedArguments.clear();
        parsedSwitches.clear();
        return false;
    }

    unnamedArguments = unnamedArgumentsList.toArray();
    return true;
}

}
