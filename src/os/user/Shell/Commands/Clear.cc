/*
 * Copyright (C) 2018 Fabian Ruhland
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

#include "Clear.h"

Clear::Clear(Shell &shell) : Command(shell) {

};

void Clear::execute(Util::Array<String> &args, OutputStream &outputStream) {
    for(uint32_t i = 1; i < args.length(); i++) {
        if(args[i] == "-h" || args[i] == "--help") {
            outputStream << "Clears the screen." << endl << endl;
            outputStream << "Usage: " << args[0] << " [OPTION]..." << endl << endl;
            outputStream << "Options:" << endl;
            outputStream << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    auto *text = Kernel::getService<GraphicsService>()->getTextDriver();
    text->clear();
}