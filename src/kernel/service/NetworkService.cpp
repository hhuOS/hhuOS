
#include <kernel/event/network/IP4SendEvent.h>
#include "kernel/core/System.h"
#include "NetworkService.h"
#include "EventBus.h"

namespace Kernel {

NetworkService::NetworkService() {
    auto *eventBus = System::getService<EventBus>();
    eventBus->subscribe(packetHandler, ReceiveEvent::TYPE);
    eventBus->subscribe(ip4Module, IP4SendEvent::TYPE);
    loopbackInterface = new Loopback();
    registerDevice(*loopbackInterface);
}

NetworkService::~NetworkService() {
    auto *eventBus = System::getService<EventBus>();
    delete loopbackInterface;
    //TODO: Synchronisierung nötig?
    eventBus->unsubscribe(packetHandler, ReceiveEvent::TYPE);
    eventBus->unsubscribe(ip4Module, IP4SendEvent::TYPE);
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
