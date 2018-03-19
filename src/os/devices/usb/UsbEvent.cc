#include "UsbEvent.h"

UsbEvent::UsbEvent() : Event(TYPE) {

}

UsbEvent::UsbEvent(uint32_t subType) : Event(TYPE) {
    this->subType = subType;
}

uint32_t UsbEvent::getSubtype() {
    return subType;
}

char *UsbEvent::getName() {
    return "USBEVENT";
}

UsbEvent::UsbEvent(const UsbEvent &other) : Event(other) {
    this->subType = other.subType;
}
