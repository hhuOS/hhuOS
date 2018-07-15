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
    String devicePath;
    String targetPath;
    String type;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            if(devicePath.isEmpty()) {
                devicePath = args[i];
            } else if(targetPath.isEmpty()) {
                targetPath = args[i];
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
            stdout << "Mounts a device to a target path." << endl << endl;
            stdout << "Usage: " << args[0] << " [DEVICE] [TARGET] [OPTIONS]..." << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -t, --type: Filesystem type (REQUIRED!)." << endl;
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

    if(type == FileSystem::TYPE_RAM_FS) {
        targetPath = devicePath;
    }

    if(devicePath.isEmpty()) {
        stderr << args[0] << ": No device given!" << endl;
        return;
    }

    if(targetPath.isEmpty()) {
        stderr << args[0] << ": No target given!" << endl;
        return;
    }

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