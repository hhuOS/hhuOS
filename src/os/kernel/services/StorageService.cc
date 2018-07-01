#include <kernel/Kernel.h>
#include <kernel/log/Logger.h>
#include "StorageService.h"
#include "devices/block/storage/Partition.h"
#include "EventBus.h"

const String StorageService::LOG_NAME = String("STORAGE SERVICE");

StorageService::StorageService() : addEventBuffer(64), removeEventBuffer(64) {

}

StorageDevice *StorageService::getDevice(const String &name) {
    if(!devices.containsKey(name)) {
        return nullptr;
    }

    return devices.get(name);
}

void StorageService::registerDevice(StorageDevice *device) {
    auto *eventBus = Kernel::getService<EventBus>();

    devices.put(device->getName(), device);

    Logger::trace(LOG_NAME, "Registering device: %s", (char *) device->getName());

    addEventBuffer.push(StorageAddEvent(device));
    eventBus->publish(addEventBuffer.pop());

    //Scan device for partitions and register them as well
    Util::Array<StorageDevice::PartitionInfo> partitions = device->readPartitionTable();

    uint8_t primaryCount = 1;
    uint8_t logicalCount = 5;
    for(StorageDevice::PartitionInfo info : partitions) {
        
        uint8_t partNumber;
        if(info.type == StorageDevice::LOGICAL)
            partNumber = logicalCount++;
        else {
            partNumber = primaryCount++;
        }

        String partitionName(device->getName() + "p" + String::valueOf(partNumber, 10));

        StorageDevice *partition = new Partition(device, info.startSector, info.sectorCount, info.systemId, partitionName);
        registerDevice(partition);
    }
}

void StorageService::removeDevice(const String &name) {
    auto *eventBus = Kernel::getService<EventBus>();

    Util::Array<String> deviceNames = devices.keySet();
    for(const String &currentName : deviceNames) {
        if(currentName.beginsWith(name)) {
            Logger::trace(LOG_NAME, "Removing device: %s", (char *) currentName);

            devices.remove(currentName);

            removeEventBuffer.push(StorageRemoveEvent(currentName));
            eventBus->publish(removeEventBuffer.pop());
        }
    }
}

StorageDevice *StorageService::findRootDevice() {
    Util::Array<String> deviceNames = devices.keySet();
    StorageDevice *ret = nullptr;

    for(const String &currentName : deviceNames) {
        StorageDevice *currentDevice = devices.get(currentName);
        if(currentDevice->getSystemId() == StorageDevice::HHU_OS_ROOT_FAT) {
            ret = currentDevice;
        }
    }

    return ret;
}
