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

    ArgumentParser parser(getHelpText(), 1);

    parser.addSwitch("help", "h");
    parser.addSwitch("bootable", "b");
    parser.addParameter("id", "i", false);
    parser.addParameter("number", "n", true);
    parser.addParameter("start", "s", true);
    parser.addParameter("end", "e", true);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(parser.getUnnamedArguments().length() == 0) {
        stderr << args[0] << ": Missing device path!" << endl;
        return;
    }

    devicePath = parser.getUnnamedArguments()[0];
    partNumber = static_cast<uint8_t>(strtoint((const char *) parser.getNamedArgument("number")));
    startSector = static_cast<uint32_t>(strtoint((const char *) parser.getNamedArgument("start")));
    endSector = static_cast<uint32_t>(strtoint((const char *) parser.getNamedArgument("end")));
    systemId = static_cast<uint8_t>(strtoint((const char *) parser.getNamedArgument("id")));

    String absoluteDevicePath = calcAbsolutePath(devicePath);
    
    if(!FileStatus::exists(absoluteDevicePath)) {
        stderr << args[0] << ": File not found '" << devicePath << "'!" << endl;
        return;
    }
    
    File *file = File::open(absoluteDevicePath, "r");
    
    StorageDevice *device = storageService->getDevice(file->getName());
    
    uint32_t ret = device->writePartition(partNumber, active, systemId, startSector, endSector - startSector);

    switch(ret) {
        case StorageDevice::SUCCESS :
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
            stderr << args[0] << ": The device is not partitionable!" << endl;
            break;
        default:
            stderr << args[0] << ": Unknown Error!" << endl;
            break;
    }

    delete file;
}

const String AddPart::getHelpText() {
    return "Adds a new partition to a device, or overwrites an existing one.\n\n"
            "Usage: addpart [OPTION]... [PATH]\n\n"
            "Options:\n"
            "  -n, --number: The partition number\n"
            "  -i, --id: The system id\n"
            "  -b, --bootable: Whether or not the partition should be bootable (true/false)\n"
            "  -s, --start: The start sector\n"
            "  -e, --end: The end sector\n"
            "  -h, --help: Show this help-message";
}
