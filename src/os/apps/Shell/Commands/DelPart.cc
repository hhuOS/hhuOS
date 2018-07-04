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
#include "DelPart.h"

DelPart::DelPart(Shell &shell) : Command(shell) {
    storageService = Kernel::getService<StorageService>();
    fileSystem = Kernel::getService<FileSystem>();
};

void DelPart::execute(Util::Array<String> &args) {
    String devicePath;
    uint8_t partNumber = 0;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            if(devicePath.isEmpty()) {
                devicePath = args[i];
            } else {
                stderr << args[0] << ": Too many arguments!" << endl;
                return;
            }
        } else if(args[i] == "-n" || args[i] == "--number") {
            if (i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            } else {
                partNumber = static_cast<uint8_t>(strtoint((const char *) args[++i]));
            }
        } else if(args[i] == "-h" || args[i] == "--help") {
            stdout << "Deletes a partition from a device." << endl << endl;
            stdout << "Usage: " << args[0] << " [OPTION]... [PATH]" << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -n, --number: The partition number" << endl;
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

    if(partNumber == 0) {
        stderr << args[0] << ": Please specify a valid partition number!" << endl;
        return;
    }

    String absoluteDevicePath = calcAbsolutePath(devicePath);

    if(!FileStatus::exists(absoluteDevicePath)) {
        stderr << args[0] << ": File not found '" << devicePath << "'!" << endl;
        return;
    }

    File &file = *File::open(absoluteDevicePath, "r");

    StorageDevice *device = storageService->getDevice(file.getName());

    uint32_t ret = device->deletePartition(partNumber);

    switch(ret) {
        case StorageDevice::SUCCESS :
            storageService->removeDevice(device->getName() + "p" + String::valueOf(partNumber, 10));
            break;
        case StorageDevice::READ_SECTOR_FAILED :
            stderr << args[0] << ": Error while reading a sector!" << endl;
            break;
        case StorageDevice::WRITE_SECTOR_FAILED :
            stderr << args[0] << ": Error while writing a sector!" << endl;
            break;
        case StorageDevice::INVALID_MBR_SIGNATURE :
            stderr << args[0] << ": The device does not contain a valid Master Boot Record!" << endl;
            break;
        case StorageDevice::EXTENDED_PARTITION_NOT_FOUND :
            stderr << args[0] << ": The device does not contain an extenden parititon!" << endl;
            break;
        default:
            stderr << args[0] << ": Unknown Error!" << endl;
            break;
    }
}