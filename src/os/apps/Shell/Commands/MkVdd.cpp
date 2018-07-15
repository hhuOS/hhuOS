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
#include <devices/block/storage/VirtualDiskDrive.h>
#include "MkVdd.h"

MkVdd::MkVdd(Shell &shell) : Command(shell) {
    storageService = Kernel::getService<StorageService>();
};

void MkVdd::execute(Util::Array<String> &args) {
    uint32_t sectorSize = 512;
    uint32_t sectorCount = 0;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(args[i] == "-s" || args[i] == "--sectorsize") {
            sectorSize = static_cast<uint32_t>(strtoint((const char *) args[++i]));
        } else if(args[i] == "-c" || args[i] == "--count") {
            sectorCount = static_cast<uint32_t>(strtoint((const char *) args[++i]));
        } else if(args[i] == "-h" || args[i] == "--help") {
            stdout << "Creates a new virtual disk drive." << endl << endl;
            stdout << "Usage: " << args[0] << " [OPTION]..." << endl << endl;
            stdout << "  -s, --sectorsize: The size of a sector on the virtual disk (Default: 512 Byte)." << endl;
            stdout << "  -c, --count: The amount of sectors, that the virtual disk has." << endl;
            stdout << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(sectorCount == 0) {
        stderr << args[0] << ": Please specify a valid amount of sectors!" << endl;
        return;
    }

    if(sectorSize < 512) {
        stderr << args[0] << ": Sector size needs to be at least 512 Bytes!" << endl;
        return;
    }

    storageService->registerDevice(new VirtualDiskDrive(sectorSize, sectorCount));
}