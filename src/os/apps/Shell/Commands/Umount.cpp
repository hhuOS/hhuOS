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

#include "Umount.h"

Umount::Umount(Shell &shell) : Command(shell) {

};

void Umount::execute(Util::Array<String> &args) {
    ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(parser.getUnnamedArguments().length() == 0) {
        stderr << args[0] << ": Missing operand!" << endl;
        return;
    }

    auto *fileSystem = Kernel::getService<FileSystem>();

    for(const String &path : parser.getUnnamedArguments()) {
        String absolutePath = calcAbsolutePath(path);
        auto ret = fileSystem->unmount(absolutePath);

        switch (ret) {
            case FileSystem::SUCCESS :
                break;
            case FileSystem::FILE_NOT_FOUND :
                stderr << args[0] << ": '" << path << "': Directory not found!" << endl;
                break;
            case FileSystem::NOTHING_MOUNTED_AT_PATH :
                stderr << args[0] << ": '" << path << "': No device mounted at target path!" << endl;
                break;
            case FileSystem::SUBDIRECTORY_CONTAINS_MOUNT_POINT:
                stderr << args[0] << ": '" << path << "': A device is still mounted to a subdirectory!" << endl;
                break;
            default:
                break;
        }
    }
}

const String Umount::getHelpText() {
    return "Unmounts devices from their mount paths.\n\n"
           "Usage: umount [PATH]...\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}
