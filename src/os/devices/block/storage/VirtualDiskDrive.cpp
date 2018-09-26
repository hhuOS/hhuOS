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

#include "VirtualDiskDrive.h"

uint32_t VirtualDiskDrive::nameCounter = 0;

VirtualDiskDrive::VirtualDiskDrive(uint32_t sectorSize, uint32_t sectorCount) :
        StorageDevice(String::format("vdd%u", nameCounter)), sectorSize(sectorSize), sectorCount(sectorCount) {
    nameCounter++;

    if(sectorSize < 512) {
        sectorSize = 512;
    }

    buffer = new uint8_t[sectorCount * sectorSize];
}

VirtualDiskDrive::~VirtualDiskDrive() {
    delete buffer;
}

String VirtualDiskDrive::getHardwareName() {
    return "hhuOS Virtual Disk Drive";
}

uint32_t VirtualDiskDrive::getSectorSize() {
    return sectorSize;
}

uint64_t VirtualDiskDrive::getSectorCount() {
    return sectorCount;
}

bool VirtualDiskDrive::read(uint8_t *buff, uint32_t sector, uint32_t count) {
    if(sector + count > sectorCount) {
        return false;
    }

    memcpy(buff, buffer + sector * sectorSize, count * sectorSize);

    return true;
}

bool VirtualDiskDrive::write(const uint8_t *buff, uint32_t sector, uint32_t count) {
    if(sector + count > sectorCount) {
        return false;
    }

    memcpy(buffer + sector * sectorSize, buff, count * sectorSize);

    return true;
}
