#include "StorageRemoveEvent.h"

StorageRemoveEvent::StorageRemoveEvent(String deviceName) : Event(TYPE) {
    this->deviceName = deviceName;
}

StorageRemoveEvent::StorageRemoveEvent() : Event(TYPE) {

}

StorageRemoveEvent::StorageRemoveEvent(const StorageRemoveEvent &other) : Event(other){
    this->deviceName = other.deviceName;
}

char *StorageRemoveEvent::getName() {
    return "StorageRemoveEvent";
}

String StorageRemoveEvent::getDeviceName() {
    return deviceName;
}
