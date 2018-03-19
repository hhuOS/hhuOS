#include "StorageAddEvent.h"

StorageAddEvent::StorageAddEvent(StorageDevice *device) : Event(TYPE) {
    this->device = device;
}

StorageAddEvent::StorageAddEvent() : Event(TYPE) {

}

StorageAddEvent::StorageAddEvent(const StorageAddEvent &other) : Event(other){
    this->device = other.device;
}

char *StorageAddEvent::getName() {
    return "StorageAddEvent";
}

StorageDevice* StorageAddEvent::getDevice() {
    return device;
}
