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

#include "StorageNode.h"

StorageNode::StorageNode(StorageDevice *disk) : VirtualNode(disk->getName(), FsNode::BLOCK_FILE){
    this->disk = disk;
}

uint64_t StorageNode::getLength() {
    return disk->getSectorCount() * disk->getSectorSize();
}

uint64_t StorageNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {

    /***************************************************************************************************************
     * NOTE:                                                                                                       *
     * This implementations downcasts pos and numBytes to 32-bit integers, because the x86-architecture            *
     * does not support 64-bit division. When this system gets ported to x86_64,                                   *
     * or supports 64-bit division in software, this code should be replaced with the commented code down below!   *
     ***************************************************************************************************************/

    auto sectorSize = disk->getSectorSize();
    auto startSector = static_cast<uint32_t>(pos) / sectorSize;
    auto count = (static_cast<uint32_t>(numBytes) / sectorSize) + 2;
    uint8_t *hddData = new uint8_t[count * sectorSize];

    if(!disk->read(hddData, startSector, count)) {
        return 0;
    }

    memcpy(buf, &hddData[static_cast<uint32_t>(pos) % sectorSize], numBytes);

    delete hddData;

    return numBytes;

    /*auto sectorSize = disk->getSectorSize();
    auto startSector = static_cast<uint32_t>(pos / sectorSize);
    auto count = static_cast<uint32_t>((numBytes / sectorSize) + 2);
    uint8_t hddData[count * sectorSize];
    
    if(!disk->read(hddData, startSector, count)) {
        return 0;
    }

    memcpy(buf, &hddData[pos % sectorSize], numBytes);

    return numBytes;*/
}

uint64_t StorageNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {

    /***************************************************************************************************************
     * NOTE:                                                                                                       *
     * This implementations downcasts pos and numBytes to 32-bit integers, because the x86-architecture            *
     * does not support 64-bit division. When this system gets ported to x86_64,                                   *
     * or supports 64-bit division in software, this code should be replaced with the commented code down below!   *
     ***************************************************************************************************************/

    auto sectorSize = disk->getSectorSize();
    auto startSector = static_cast<uint32_t>(pos) / sectorSize;
    auto lastSector = (static_cast<uint32_t>(startSector) + (static_cast<uint32_t>(numBytes) / sectorSize) + 1);
    auto count = (static_cast<uint32_t>(numBytes) / sectorSize) + 2;
    uint8_t *hddData = new uint8_t[count * sectorSize];

    //Read the first and last sector of the affected area.
    if(!disk->read(hddData, startSector, 1)) {
        return 0;
    }

    if(!disk->read(&hddData[(count - 1) * sectorSize], lastSector, 1)) {
        return 0;
    }

    //Copy the data at the appropriate space in hddData.
    memcpy(&hddData[static_cast<uint32_t>(pos) % sectorSize], buf, numBytes);

    //Write hddData. This way, no data on the AHCI-device is lost/overwritten.
    if(!disk->write(hddData, startSector, count)) {
        return 0;
    }

    delete[] hddData;

    return numBytes;

    /*auto sectorSize = disk->getSectorSize();
    auto startSector = static_cast<uint32_t>(pos / sectorSize);
    auto lastSector = static_cast<uint32_t>(startSector + (numBytes / sectorSize) + 1);
    auto count = static_cast<uint32_t>((numBytes / sectorSize) + 2);
    uint8_t hddData[count * sectorSize];

    //Read the first and last sector of the affected area.
    if(!disk->read(hddData, startSector, 1)) {
        return 0;
    }

    if(!disk->read(&hddData[(count - 1) * sectorSize], lastSector, 1)) {
        return 0;
    }

    //Copy the data at the appropriate space in hddData.
    memcpy(&hddData[pos % sectorSize], buf, numBytes);

    //Write hddData. This way, no data on the AHCI-device is lost/overwritten.
    if(!disk->write(hddData, startSector, count)) {
        return 0;
    }

    return numBytes;*/
}