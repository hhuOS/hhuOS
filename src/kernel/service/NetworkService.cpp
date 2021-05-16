
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include "kernel/core/System.h"
#include "NetworkService.h"
#include "EventBus.h"

namespace Kernel {

    NetworkService::NetworkService() {
        auto *eventBus = System::getService<EventBus>();

        ethernetModule = new EthernetModule();
        ip4Module = new IP4Module();

        eventBus->subscribe(*ip4Module, IP4ReceiveEvent::TYPE);
        eventBus->subscribe(*ip4Module, ARPReceiveEvent::TYPE);
        eventBus->subscribe(*ethernetModule, EthernetReceiveEvent::TYPE);
        eventBus->subscribe(packetHandler, ReceiveEvent::TYPE);

        eventBus->subscribe(*ethernetModule, EthernetSendEvent::TYPE);
        eventBus->subscribe(*ip4Module, IP4SendEvent::TYPE);

        loopbackInterface = new Loopback();
        registerDevice(*loopbackInterface);
    }

    NetworkService::~NetworkService() {
        auto *eventBus = System::getService<EventBus>();
        //TODO: Synchronisierung nötig?
        eventBus->unsubscribe(*ip4Module, IP4SendEvent::TYPE);
        eventBus->unsubscribe(*ethernetModule, EthernetSendEvent::TYPE);

        eventBus->unsubscribe(packetHandler, ReceiveEvent::TYPE);
        eventBus->unsubscribe(*ethernetModule, EthernetReceiveEvent::TYPE);
        eventBus->unsubscribe(*ip4Module, ARPReceiveEvent::TYPE);
        eventBus->unsubscribe(*ip4Module, IP4ReceiveEvent::TYPE);

        delete ip4Module;
        delete ethernetModule;
        delete loopbackInterface;
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
