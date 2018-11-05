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
#include "MkVdd.h"

MkVdd::MkVdd(Shell &shell) : Command(shell) {
    storageService = Kernel::getService<StorageService>();
};

void MkVdd::execute(Util::Array<String> &args) {
    uint32_t sectorSize = 512;

    ArgumentParser parser(getHelpText(), 1);
    parser.addParameter("sectorsize", "s", false);
    parser.addParameter("count", "c", true);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    uint32_t sectorCount = static_cast<uint32_t>(strtoint((const char*) parser.getNamedArgument("count")));

    if(!parser.getNamedArgument("sectorsize").isEmpty()) {
        sectorSize = static_cast<uint32_t>(strtoint((const char *) parser.getNamedArgument("sectorsize")));
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

const String MkVdd::getHelpText() {
    return "Creates a new virtual disk drive.\n\n"
           "Usage: mkvdd [OPTION]...\n\n"
           "  -s, --sectorsize: The size of a sector on the virtual disk (Default: 512 Byte).\n"
           "  -c, --count: The amount of sectors, that the virtual disk has.\n"
           "  -h, --help: Show this help-message.";
}
