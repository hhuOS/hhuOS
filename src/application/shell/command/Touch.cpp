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

#include "lib/file/FileStatus.h"
#include "lib/file/File.h"
#include "Touch.h"

Touch::Touch(Shell &shell) : Command(shell) {

};

void Touch::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(parser.getUnnamedArguments().length() == 0) {
        stderr << args[0] << ": Missing operand!" << endl;
        return;
    }

    fileSystem = Kernel::System::getService<Filesystem>();

    for(const String &path : parser.getUnnamedArguments()) {
        String absolutePath = calcAbsolutePath(path);

        if(!FileStatus::exists(absolutePath)) {
            auto ret = fileSystem->createFile(absolutePath);

            switch(ret) {
                case Filesystem::SUCCESS :
                    break;
                case Filesystem::FILE_NOT_FOUND :
                    stderr << args[0] << ": '" << path << "': File or directory not found!" << endl;
                    break;
                case Filesystem::CREATING_FILE_FAILED :
                    stderr << args[0] << ": '" << path << "': Unable to create file!" << endl;
                    break;
                default:
                    break;
            }
        }
    }
}

const String Touch::getHelpText() {
    return "Creates new files.\n\n"
           "Usage: touch [OPTION]... [PATH]...\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}
