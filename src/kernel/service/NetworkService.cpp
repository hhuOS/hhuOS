
#include "kernel/core/Kernel.h"
#include "NetworkService.h"
#include "EventBus.h"

NetworkService::NetworkService() {
    Kernel::getService<EventBus>()->subscribe(packetHandler, ReceiveEvent::TYPE);
}

NetworkService::~NetworkService() {
    Kernel::getService<EventBus>()->unsubscribe(packetHandler, ReceiveEvent::TYPE);
}

uint32_t NetworkService::getDeviceCount() {
    return drivers.size();
}

NetworkDevice& NetworkService::getDriver(uint8_t index) {
    return *drivers.get(index);
}

void NetworkService::removeDevice(uint8_t index) {
    drivers.remove(index);
}

void NetworkService::registerDevice(NetworkDevice &driver) {
    drivers.add(&driver);
}
