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
        String currentArg = argv[i];

        if (!currentArg.beginsWith("-") || currentArg == "-") {
            unnamedArgumentsList.add(currentArg);
        } else if (currentArg == "-h" || currentArg == "--help") {
            errorString = helpText;
            namedArguments.clear();
            parsedSwitches.clear();
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
                    namedArguments.clear();
                    parsedSwitches.clear();
                    return false;
                }
            } else if (switches.contains(currentArg)) {
                parsedSwitches.add(currentArg);
            } else {
                errorString = "Unknown parameter '" + currentArg + "'!";
                namedArguments.clear();
                parsedSwitches.clear();
                return false;
            }
        }
    }

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
