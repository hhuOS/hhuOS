#include "StorageRemoveEvent.h"

StorageRemoveEvent::StorageRemoveEvent() : Event(TYPE) {

}

StorageRemoveEvent::StorageRemoveEvent(String deviceName) : Event(TYPE) {
    this->deviceName = deviceName;
}

StorageRemoveEvent::StorageRemoveEvent(const StorageRemoveEvent &other) : Event(other) {
    this->deviceName = other.deviceName;
}

char *StorageRemoveEvent::getName() {
    return const_cast<char *>("StorageRemoveEvent");
}

String StorageRemoveEvent::getDeviceName() {
    return deviceName;
}
