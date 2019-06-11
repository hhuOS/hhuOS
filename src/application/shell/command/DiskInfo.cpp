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

#include "lib/libc/printf.h"
#include "lib/file/FileStatus.h"
#include "DiskInfo.h"

DiskInfo::DiskInfo(Shell &shell) : Command(shell) {
    storageService = Kernel::System::getService<Kernel::StorageService>();
    fileSystem = Kernel::System::getService<Filesystem>();
};

void DiskInfo::execute(Util::Array<String> &args) {
    Util::ArrayList<String> devicePaths;

    Util::ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    for(const String &path : parser.getUnnamedArguments()) {
        devicePaths.add(path);
    }

    if(devicePaths.isEmpty()) {
        Directory *dir = Directory::open("/dev/storage");

        for(const String &deviceName : dir->getChildren()) {
            devicePaths.add("/dev/storage/" + deviceName);
        }

        delete dir;
    }

    for(const String &devicePath : devicePaths) {

        String absoluteDevicePath = calcAbsolutePath(devicePath);

        if (!FileStatus::exists(absoluteDevicePath)) {
            stderr << args[0] << ": File or Directory not found!" << endl;
            return;
        }

        FileStatus *fStat = FileStatus::stat(absoluteDevicePath);

        StorageDevice *device = storageService->getDevice(fStat->getName());

        if (device == nullptr) {
            stderr << args[0] << ": Unable to open device '" << fStat->getName() << "'!" << endl;
            return;
        }

        stdout << fStat->getAbsolutePath() << ":" << endl;
        stdout << "  Device name:  " << device->getHardwareName() << endl;
        stdout << "  Sector size:  " << dec << device->getSectorSize() << endl;
        stdout << "  Sector count: " << dec << (unsigned long) device->getSectorCount() << endl;
        stdout << "  System ID:    " << hex << (unsigned int) device->getSystemId() << dec << endl;

        delete fStat;
    }
}

const String DiskInfo::getHelpText() {
    return "Shows information about a storage device.\n\n"
           "Usage: diskinfo [DEVICE]... [OPTION]...\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}
