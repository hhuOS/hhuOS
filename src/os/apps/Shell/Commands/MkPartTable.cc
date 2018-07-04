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

#include <lib/libc/printf.h>
#include <lib/file/FileStatus.h>
#include <lib/file/File.h>
#include <devices/block/storage/Partition.h>
#include "MkPartTable.h"

MkPartTable::MkPartTable(Shell &shell) : Command(shell) {
    storageService = Kernel::getService<StorageService>();
    fileSystem = Kernel::getService<FileSystem>();
};

void MkPartTable::execute(Util::Array<String> &args) {
    String devicePath;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            if(devicePath.isEmpty()) {
                devicePath = args[i];
            } else {
                stderr << args[0] << ": Too many arguments!" << endl;
                return;
            }
        } else if(args[i] == "-h" || args[i] == "--help") {
            stdout << "Adds a new partition to a device, or overwrite an existing one.." << endl << endl;
            stdout << "Usage: " << args[0] << " [PATH]" << endl << endl;
            stdout << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(devicePath.isEmpty()) {
        stderr << args[0] << ": No device given!" << endl;
        return;
    }

    String absoluteDevicePath = calcAbsolutePath(devicePath);

    if(!FileStatus::exists(absoluteDevicePath)) {
        stderr << args[0] << ": File not found '" << devicePath << "'!" << endl;
        return;
    }

    File &file = *File::open(absoluteDevicePath, "r");

    StorageDevice *device = storageService->getDevice(file.getName());

    uint32_t ret = device->createPartitionTable();

    switch(ret) {
        case StorageDevice::SUCCESS : {
            Directory &dir = *Directory::open("/dev/storage");

            for (const String &name : dir.getChildren()) {
                if (name != device->getName() && name.beginsWith(device->getName())) {
                    storageService->removeDevice(name);
                }
            }

            break;
        }
        case StorageDevice::READ_SECTOR_FAILED :
            stderr << args[0] << ": Error while reading a sector!" << endl;
            break;
        case StorageDevice::WRITE_SECTOR_FAILED :
            stderr << args[0] << ": Error while writing a sector!" << endl;
            break;
        case StorageDevice::INVALID_MBR_SIGNATURE :
            stderr << args[0] << ": The device does not contain a valid Master Boot Record!" << endl;
            break;
        case StorageDevice::DEVICE_NOT_PARTITIONABLE :
            stderr << args[0] << ": The device '" << devicePath << "' is not partitionable!" << endl;
            break;
        default:
            stderr << args[0] << ": Unknown Error!" << endl;
            break;
    }
}