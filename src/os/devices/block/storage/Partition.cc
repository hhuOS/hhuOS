#include "Partition.h"

Partition::Partition(StorageDevice *parentDevice, uint32_t startSector, uint32_t sectorCount, uint8_t systemId, String &name) :
StorageDevice(name), parentDevice(parentDevice), startSector(startSector), sectorCount(sectorCount), systemId(systemId) {

}

Util::Array<StorageDevice::PartitionInfo> Partition::readPartitionTable() {
    return StorageDevice::partitionList.toArray();
}

uint32_t Partition::writePartition(uint8_t partNumber, bool active, uint8_t systemId, uint32_t startSector, uint32_t sectorCount) {
    return DEVICE_NOT_PARTITIONABLE;
}

uint32_t Partition::deletePartition(uint8_t partNumber) {
    return DEVICE_NOT_PARTITIONABLE;
}

uint32_t Partition::createPartitionTable() {
    return DEVICE_NOT_PARTITIONABLE;
}

uint8_t Partition::getSystemId() {
    return systemId;
}

bool Partition::read(uint8_t *buff, uint32_t sector, uint32_t count) {
    if(count > sectorCount)
        return false;

    return parentDevice->read(buff, startSector + sector, count);
}

bool Partition::write(const uint8_t *buff, uint32_t sector, uint32_t count) {
    if(count > sectorCount)
        return false;
        
    return parentDevice->write(buff, startSector + sector, count);
}

String Partition::getDeviceName() {
    return "Partition on " + parentDevice->getDeviceName();
}

uint32_t Partition::getSectorSize() {
    return parentDevice->getSectorSize();
}

uint64_t Partition::getSectorCount() {
    return sectorCount;
}
