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
    String devicePath;
    String type;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            if(devicePath.isEmpty()) {
                devicePath = args[i];
            } else {
                stderr << args[0] << ": Too many arguments!" << endl;
                return;
            }
        } else if(args[i] == "-t" || args[i] == "--type") {
            if(i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            } else {
                type = args[++i];
            }
        } else if(args[i] == "-h" || args[i] == "--help") {
            stdout << "Creates new filesystems." << endl << endl;
            stdout << "Usage: " << args[0] << " [OPTION]... [PATH]..." << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -t, --type: The filesystem type." << endl;
            stdout << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(type.isEmpty()) {
        stderr << args[0] << ": No filesystem type given!" << endl;
        return;
    }

    if(devicePath.isEmpty()) {
        stderr << args[0] << ": No device given!" << endl;
        return;
    }

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