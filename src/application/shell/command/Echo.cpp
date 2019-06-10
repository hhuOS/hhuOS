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

#include "lib/util/ArgumentParser.h"
#include "Echo.h"

Echo::Echo(Shell &shell) : Command(shell) {

};

void Echo::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    parser.addSwitch("help", "h");

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    for(const String &string : parser.getUnnamedArguments()) {
        stdout << string << " ";
    }

    stdout << endl;
}

const String Echo::getHelpText() {
    return "Writes it's arguments to the standard output stream.\n\n"
            "Usage: echo [OPTION]... [ARGUMENT]...\n"
            "Options:\n"
            "  -h, --help: Show this help-message.";
}
