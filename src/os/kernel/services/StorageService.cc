#include "StorageService.h"
#include "devices/block/storage/Partition.h"
#include "lib/libc/printf.h"

StorageDevice *StorageService::getDevice(const String &name) {
    return devices.get(name).value();
}

void StorageService::registerDevice(StorageDevice *device) {
    EventBus *eventBus = Kernel::getService<EventBus>();

    devices.put(device->getName(), device);

    STORAGE_SERVICE_TRACE("Registering device: %s\n", (char *) device->getName());

    auto *event = new StorageAddEvent(device);
    eventBus->publish(*event);

    //Scan device for partitions and register them as well
    Util::ArrayList<StorageDevice::PartitionInfo>& partitionList = device->readPartitionTable();

    uint8_t primaryCount = 1;
    uint8_t logicalCount = 5;
    for(StorageDevice::PartitionInfo info : partitionList) {
        
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
    EventBus *eventBus = Kernel::getService<EventBus>();

    List<String> *deviceNames = devices.keySet();
    for(const String &currentName : *deviceNames) {
        if(currentName.beginsWith(name)) {
            STORAGE_SERVICE_TRACE("Removing device: %s\n", (char *) currentName);

            devices.remove(currentName);

            StorageRemoveEvent *event = new StorageRemoveEvent(currentName);
            eventBus->publish(*event);
        }
    }

    delete deviceNames;
}

StorageDevice *StorageService::findRootDevice() {
    List<StorageDevice *> *deviceList = devices.valueSet();
    StorageDevice *ret = nullptr;

    for(StorageDevice *device : *deviceList) {
        if(device->getSystemId() == StorageDevice::HHU_OS_ROOT_FAT) {
            ret = device;
        }
    }

    delete deviceList;
    return ret;
}
