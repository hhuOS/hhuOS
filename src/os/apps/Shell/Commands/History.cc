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

void History::execute(Util::Array<String> &args) {
    bool clear = false;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(args[i] == "-c" || args[i] == "--clear") {
            clear = true;
        } else if(args[i] == "-h" || args[i] == "--help") {
            stdout << "Display the command history." << endl << endl;
            stdout << "Usage: " << args[0] << " [OPTION]..." << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -c, --clear: Clear the history." << endl;
            stdout << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(clear) {
        shell.history.clear();
        return;
    }

    for(uint32_t i = 0; i < shell.history.size(); i++) {
        stdout << i << " " << shell.history.get(i) << endl;
    }
}