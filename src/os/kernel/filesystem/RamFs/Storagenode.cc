#include "StorageNode.h"

#define SECTOR_SIZE     512

uint64_t StorageNode::getLength() {
    return disk->getSectorCount() * 512;
}

char *StorageNode::readData(char *buf, uint64_t pos, uint32_t numBytes) {
    uint64_t startSector = pos / SECTOR_SIZE;
    uint32_t count = (numBytes / SECTOR_SIZE) + 2;
    uint8_t hddData[count * SECTOR_SIZE];
    
    if(!disk->read(hddData, startSector, count))
        return nullptr;
    
    memcpy(buf, &hddData[pos % SECTOR_SIZE], numBytes);
    return buf;
}

int32_t StorageNode::writeData(char *buf, uint64_t pos, uint32_t numBytes) {
    uint64_t startSector = pos / SECTOR_SIZE;
    uint64_t lastSector = startSector + (numBytes / SECTOR_SIZE) + 1;
    uint32_t count = (numBytes / SECTOR_SIZE) + 2;
    uint8_t hddData[count * SECTOR_SIZE];
    
    //Read the first and last sector of the affected area.
    if(!disk->read(hddData, startSector, 1))
        return -1;
    if(!disk->read(&hddData[(count - 1) * SECTOR_SIZE], lastSector, 1))
        return -1;
    
    //Copy the data at the appropriate space in hddData.
    memcpy(&hddData[pos % SECTOR_SIZE], buf, numBytes);
    
    //Write hddData. This way, no data on the AHCI-device is lost/overwritten.
    if(!disk->write(hddData, startSector, count))
        return -1;
    
    return 0;
}