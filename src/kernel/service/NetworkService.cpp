
#include "kernel/core/System.h"
#include "NetworkService.h"
#include "EventBus.h"

namespace Kernel {

NetworkService::NetworkService() {
    System::getService<EventBus>()->subscribe(packetHandler, ReceiveEvent::TYPE);
    loopbackInterface = new Loopback();
    registerDevice(*loopbackInterface);
}

NetworkService::~NetworkService() {
    delete loopbackInterface;
    //TODO: Synchronisierung nötig?
    System::getService<EventBus>()->unsubscribe(packetHandler, ReceiveEvent::TYPE);
}

uint32_t NetworkService::getDeviceCount() {
    return drivers.size();
}

NetworkDevice &NetworkService::getDriver(uint8_t index) {
    return *drivers.get(index);
}

void NetworkService::removeDevice(uint8_t index) {
    drivers.remove(index);
}

void NetworkService::registerDevice(NetworkDevice &driver) {
    drivers.add(&driver);
}

}
