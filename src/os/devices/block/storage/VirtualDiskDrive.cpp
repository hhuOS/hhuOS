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
