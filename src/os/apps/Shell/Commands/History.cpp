/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "History.h"

History::History(Shell &shell) : Command(shell) {

};

void History::execute(Util::Array<String> &args) {ArgumentParser parser(getHelpText(), 1);
    parser.addSwitch("clear", "c");

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(parser.checkSwitch("clear")) {
        shell.history.clear();
        return;
    }

    for(uint32_t i = 0; i < shell.history.size(); i++) {
        stdout << "  " << i << " " << shell.history.get(i) << endl;
    }
}

const String History::getHelpText() {
    return "Display the command history.\n\n"
            "Usage: history [OPTION]...\n\n"
            "Options:\n"
            "  -c, --clear: Clear the history.\n"
            "  -h, --help: Show this help-message.";
}
