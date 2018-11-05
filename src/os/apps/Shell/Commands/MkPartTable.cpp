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
#include "MkPartTable.h"

MkPartTable::MkPartTable(Shell &shell) : Command(shell) {
    storageService = Kernel::getService<StorageService>();
    fileSystem = Kernel::getService<FileSystem>();
};

void MkPartTable::execute(Util::Array<String> &args) {
    ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(parser.getUnnamedArguments().length() == 0) {
        stderr << args[0] << ": No device given!" << endl;
        return;
    }

    String devicePath = parser.getUnnamedArguments()[0];

    String absoluteDevicePath = calcAbsolutePath(devicePath);

    if(!FileStatus::exists(absoluteDevicePath)) {
        stderr << args[0] << ": File not found '" << devicePath << "'!" << endl;
        return;
    }

    File *file = File::open(absoluteDevicePath, "r");

    StorageDevice *device = storageService->getDevice(file->getName());

    uint32_t ret = device->createPartitionTable();

    switch(ret) {
        case StorageDevice::SUCCESS : {
            break;
        }
        case StorageDevice::WRITE_SECTOR_FAILED :
            stderr << args[0] << ": Error while writing a sector!" << endl;
            break;
        default:
            stderr << args[0] << ": Unknown Error!" << endl;
            break;
    }

    delete file;
}

const String MkPartTable::getHelpText() {
    return "Creates a new (empty) partition table on a given device..\n\n"
           "Usage: mkparttable [PATH]\n\n"
           "  -h, --help: Show this help-message.";
}
