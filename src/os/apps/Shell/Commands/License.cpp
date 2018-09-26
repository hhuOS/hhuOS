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

#include "License.h"

License::License(Shell &shell) : Command(shell) {

}

void License::execute(Util::Array<String> &args) {
    ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    stdout << "Copyright (C) 2018  Burak Akguel, Christian Gesse, Fabian Ruhland," << endl;
    stdout << "Filip Krakowski, Michael Schoettner - Heinrich-Heine University" << endl << endl;
    stdout << "This program is free software: you can redistribute it and/or modify" << endl;
    stdout << "it under the terms of the GNU General Public License as published by" << endl;
    stdout << "the Free Software Foundation, either version 3 of the License, or" << endl;
    stdout << "(at your option) any later version." << endl << endl;
    stdout << "This program is distributed in the hope that it will be useful," << endl;
    stdout << "but WITHOUT ANY WARRANTY; without even the implied warranty of" << endl;
    stdout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" << endl;
    stdout << "GNU General Public License for more details." << endl << endl;
    stdout << "You should have received a copy of the GNU General Public License" << endl;
    stdout << "along with this program.  If not, see <http://www.gnu.org/licenses/>." << endl;
}

const String License::getHelpText() {
    return "Shows the copyright license.\n\n"
           "Usage: license [OPTION]... \n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}
