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
#include "DiskInfo.h"

DiskInfo::DiskInfo(Shell &shell) : Command(shell) {
    storageService = Kernel::getService<StorageService>();
    fileSystem = Kernel::getService<FileSystem>();
};

void DiskInfo::execute(Util::Array<String> &args) {
    Util::ArrayList<String> devicePaths;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            devicePaths.add(args[i]);
        } else if(args[i] == "-h" || args[i] == "--help") {
            stdout << "Shows information about a storage device." << endl << endl;
            stdout << "Usage: " << args[0] << "[DEVICE]... [OPTION]..." << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(devicePaths.isEmpty()) {
        Directory &dir = *Directory::open("/dev/storage");

        for(const String &deviceName : dir.getChildren()) {
            devicePaths.add("/dev/storage/" + deviceName);
        }
    }

    for(const String &devicePath : devicePaths) {

        String absoluteDevicePath = calcAbsolutePath(devicePath);

        if (!FileStatus::exists(absoluteDevicePath)) {
            stderr << args[0] << ": File or Directory not found!" << endl;
            return;
        }

        FileStatus &fStat = *FileStatus::stat(absoluteDevicePath);

        StorageDevice *device = storageService->getDevice(fStat.getName());

        if (device == nullptr) {
            stderr << args[0] << ": Unable to open device '" << fStat.getName() << "'!" << endl;
            return;
        }

        stdout << fStat.getAbsolutePath() << ":" << endl;
        stdout << "  Device name:  " << device->getDeviceName() << endl;
        stdout << "  Sector size:  " << dec << device->getSectorSize() << endl;
        stdout << "  Sector count: " << dec << (unsigned long) device->getSectorCount() << endl;
        stdout << "  System ID:    " << hex << (unsigned int) device->getSystemId() << dec << endl;
    }
}
