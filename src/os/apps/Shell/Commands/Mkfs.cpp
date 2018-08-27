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

#include <lib/file/FileStatus.h>
#include <lib/file/File.h>
#include "Mkfs.h"

Mkfs::Mkfs(Shell &shell) : Command(shell) {
    fileSystem = Kernel::getService<FileSystem>();
};

void Mkfs::execute(Util::Array<String> &args) {
    ArgumentParser parser(getHelpText(), 1);
    parser.addParameter("type", "t", true);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(parser.getUnnamedArguments().length() == 0) {
        stderr << args[0] << ": No device given!" << endl;
        return;
    }

    String type = parser.getNamedArgument("type");
    String devicePath = parser.getUnnamedArguments()[0];

    String absoluteDevicePath = calcAbsolutePath(devicePath);

    uint32_t ret = fileSystem->createFilesystem(absoluteDevicePath, type);

    switch(ret) {
        case FileSystem::SUCCESS :
            break;
        case FileSystem::DEVICE_NOT_FOUND :
            stderr << args[0] << ": '" << devicePath << "': Device not found!" << endl;
            break;
        case FileSystem::INVALID_DRIVER :
            stderr << args[0] << ": '" << type << "': Unknown filesystem type!" << endl;
            break;
        case FileSystem::FORMATTING_FAILED :
            stderr << args[0] << ": '" << devicePath << "': Error while creating the filesystem!" << endl;
            break;
        default:
            break;
    }
}

const String Mkfs::getHelpText() {
    return "Creates new filesystems.\n\n"
           "Usage: mkfs [OPTION]... [PATH]...\n\n"
           "Options:\n"
           "  -t, --type: The filesystem type.\n"
           "  -h, --help: Show this help-message.";
}
