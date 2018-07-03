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
#include "AddPart.h"

AddPart::AddPart(Shell &shell) : Command(shell) {
    storageService = Kernel::getService<StorageService>();
    fileSystem = Kernel::getService<FileSystem>();
};

void AddPart::execute(Util::Array<String> &args) {
    String devicePath;
    uint8_t partNumber = 0;
    bool active = false;
    uint8_t systemId = StorageDevice::HHU_OS;
    uint32_t startSector = 0;
    uint32_t endSector = 0;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            if(devicePath.isEmpty()) {
                devicePath = args[i];
            } else {
                stderr << args[0] << ": Too many arguments!" << endl;
                return;
            }
        } else if(args[i] == "-b" || args[i] == "--bootable") {
            if(i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            } else {
                String arg = args[++i];
                active = arg == "true";
            }
        } else if(args[i] == "-n" || args[i] == "--number") {
            if (i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            } else {
                partNumber = static_cast<uint8_t>(strtoint((const char *) args[++i]));
            }
        } else if(args[i] == "-i" || args[i] == "--id") {
            if (i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            } else {
                systemId = static_cast<uint8_t>(strtoint((const char *) args[++i]));
            }
        } else if(args[i] == "-s" || args[i] == "--start") {
            if (i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            } else {
                startSector = static_cast<uint32_t>(strtoint((const char *) args[++i]));
            }
        } else if(args[i] == "-e" || args[i] == "--end") {
            if (i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            } else {
                endSector = static_cast<uint32_t>(strtoint((const char *) args[++i]));
            }
        } else if(args[i] == "-h" || args[i] == "--help") {
            stdout << "Adds a new partition to a device, or overwrite an existing one.." << endl << endl;
            stdout << "Usage: " << args[0] << " [OPTION]... [PATH]..." << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -n, --number: The partition number" << endl;
            stdout << "  -i, --id: The system id" << endl;
            stdout << "  -b, --bootable: Whether or not the partition should be bootable (true/false)" << endl;
            stdout << "  -s, --start: The start sector" << endl;
            stdout << "  -e, --end: The end sector" << endl;
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
    
    if(startSector == 0) {
        stderr << args[0] << ": Please specify a valid start sector!" << endl;
        return;
    }

    if(endSector  == 0) {
        stderr << args[0] << ": Please specify a valid end sector!" << endl;
        return;
    }

    String absoluteDevicePath = calcAbsolutePath(devicePath);
    
    if(!FileStatus::exists(absoluteDevicePath)) {
        stderr << args[0] << ": File not found '" << devicePath << "'!" << endl;
        return;
    }
    
    File &file = *File::open(absoluteDevicePath, "r");
    
    StorageDevice *device = storageService->getDevice(file.getName());
    
    uint32_t ret = device->writePartition(partNumber, active, systemId, startSector, endSector - startSector);

    switch(ret) {
        case StorageDevice::SUCCESS :
            storageService->registerDevice(new Partition(device, startSector, endSector - startSector, systemId, device->getName() + "p" + String::valueOf(partNumber)));
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
        case StorageDevice::DEVICE_NOT_PARTITIONABLE :
            stderr << args[0] << ": The device '" << devicePath << "' is not partitionable!" << endl;
            break;
        default:
            stderr << args[0] << ": Unknown Error!" << endl;
            break;
    }
}