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

#include "Mount.h"

Mount::Mount(Shell &shell) : Command(shell) {

};

void Mount::execute(Util::Array<String> &args) {
    ArgumentParser parser(getHelpText(), 1);
    parser.addParameter("type", "t", true);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(parser.getUnnamedArguments().length() < 2) {
        stderr << args[0] << ": Please enter valid device and target paths!" << endl;
        return;
    }

    String type = parser.getNamedArgument("type");
    String devicePath = parser.getUnnamedArguments()[0];
    String targetPath = parser.getUnnamedArguments()[1];

    String absoluteDevicePath = calcAbsolutePath(devicePath);
    String absoluteTargetPath = calcAbsolutePath(targetPath);

    auto *fileSystem = Kernel::getService<FileSystem>();
    auto ret = fileSystem->mount(absoluteDevicePath, absoluteTargetPath, type);

    switch(ret) {
        case FileSystem::SUCCESS :
            break;
        case FileSystem::DEVICE_NOT_FOUND :
            stderr << args[0] << ": '" << devicePath << "': Device not found!" << endl;
            break;
        case FileSystem::FILE_NOT_FOUND :
            stderr << args[0] << ": '" << targetPath << "': Directory not found!" << endl;
            break;
        case FileSystem::MOUNT_TARGET_ALREADY_USED :
            stderr << args[0] << ": '" << targetPath << "': A device is already mounted to that path!" << endl;
            break;
        case FileSystem::INVALID_DRIVER :
            stderr << args[0] << ": '" << type << "': Unknown filesystem type!" << endl;
            break;
        case FileSystem::MOUNTING_FAILED :
            stderr << args[0] << ": Unable to mount '" << devicePath << "' to '" << targetPath << "'!" << endl;
            break;
        default:
            break;
    }
}

const String Mount::getHelpText() {
    return "Mounts a device to a target path.\n\n"
           "Usage: mount [DEVICE] [TARGET] [OPTIONS]...\n\n"
           "Options:\n"
           "  -t, --type: Filesystem type (REQUIRED!).\n"
           "  -h, --help: Show this help-message.";
}
