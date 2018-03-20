#include "StorageNode.h"

#define SECTOR_SIZE     512

uint64_t StorageNode::getLength() {
    return disk->getSectorCount() * 512;
}

bool StorageNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    auto startSector = static_cast<uint32_t>(pos / SECTOR_SIZE);
    auto count = static_cast<uint32_t>((numBytes / SECTOR_SIZE) + 2);
    uint8_t hddData[count * SECTOR_SIZE];
    
    if(!disk->read(hddData, startSector, count)) {
        return false;
    }
    
    memcpy(buf, &hddData[pos % SECTOR_SIZE], numBytes);

    return true;
}

bool StorageNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    auto startSector = static_cast<uint32_t>(pos / SECTOR_SIZE);
    auto lastSector = static_cast<uint32_t>(startSector + (numBytes / SECTOR_SIZE) + 1);
    auto count = static_cast<uint32_t>((numBytes / SECTOR_SIZE) + 2);
    uint8_t hddData[count * SECTOR_SIZE];
    
    //Read the first and last sector of the affected area.
    if(!disk->read(hddData, startSector, 1)) {
        return false;
    }

    if(!disk->read(&hddData[(count - 1) * SECTOR_SIZE], lastSector, 1)) {
        return false;
    }
    
    //Copy the data at the appropriate space in hddData.
    memcpy(&hddData[pos % SECTOR_SIZE], buf, numBytes);
    
    //Write hddData. This way, no data on the AHCI-device is lost/overwritten.
    if(!disk->write(hddData, startSector, count)) {
        return false;
    }
    
    return true;
}