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

#include <lib/libc/printf.h>
#include <lib/file/FileStatus.h>
#include <lib/file/File.h>
#include <devices/storage/devices/Partition.h>
#include <devices/storage/devices/VirtualDiskDrive.h>
#include "DelVdd.h"

DelVdd::DelVdd(Shell &shell) : Command(shell) {
    storageService = Kernel::getService<StorageService>();
};

void DelVdd::execute(Util::Array<String> &args) {
    String devicePath;

    ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(parser.getUnnamedArguments().length() == 0) {
        stderr << args[0] << ": Missing device path!" << endl;
        return;
    }

    devicePath = parser.getUnnamedArguments()[0];

    if(devicePath.isEmpty()) {
        stderr << args[0] << ": No device given!" << endl;
        return;
    }

    String absoluteDevicePath = calcAbsolutePath(devicePath);

    if(!FileStatus::exists(absoluteDevicePath)) {
        stderr << args[0] << ": File not found '" << devicePath << "'!" << endl;
        return;
    }

    File *file = File::open(absoluteDevicePath, "r");

    if(!file->getName().beginsWith("vdd")) {
        stderr << args[0] << ": Please specify a path to a valid virtual disk drive!" << endl;

        delete file;
        return;
    }

    // Remove the device and all it's partitions from storage service
    Directory *dir = Directory::open("/dev/storage");
    auto *storageService = Kernel::getService<StorageService>();

    for (const String &name : dir->getChildren()) {
        if (name.beginsWith(file->getName())) {
            storageService->removeDevice(name);
        }
    }

    delete file;
    delete dir;
}

const String DelVdd::getHelpText() {
    return "Removes a virtual disk drive.\n\n"
           "Usage: delvdd [DEVICE] [OPTION]...\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message";
}
