#include "StorageAddEvent.h"

StorageAddEvent::StorageAddEvent() : Event(TYPE) {

}

StorageAddEvent::StorageAddEvent(StorageDevice *device) : Event(TYPE) {
    this->device = device;
}

StorageAddEvent::StorageAddEvent(const StorageAddEvent &other) : Event(other){
    this->device = other.device;
}

char *StorageAddEvent::getName() {
    return const_cast<char *>("StorageAddEvent");
}

StorageDevice* StorageAddEvent::getDevice() {
    return device;
}
