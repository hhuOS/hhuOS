
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include <kernel/network/ethernet/EthernetDevice.h>
#include "kernel/core/System.h"
#include "NetworkService.h"
#include "EventBus.h"

namespace Kernel {

    NetworkService::NetworkService() {
        loopbackIdentifier = new String("lo");
        eventBus = System::getService<EventBus>();

        ethernetModule = new EthernetModule(eventBus);
        ip4Module = new IP4Module(eventBus);

        //Setup Loopback with 127.0.0.1/8
        registerDevice(loopbackIdentifier, *(new Loopback(eventBus)));
        assignIP4Address(
                loopbackIdentifier,
                new IP4Address(127, 0, 0, 1),
                new IP4Netmask(8)
        );

//        eventBus->subscribe(*ip4Module, IP4ReceiveEvent::TYPE);
//        eventBus->subscribe(*ip4Module, ARPReceiveEvent::TYPE);
//        eventBus->subscribe(*ethernetModule, EthernetReceiveEvent::TYPE);
        eventBus->subscribe(packetHandler, ReceiveEvent::TYPE);

        eventBus->subscribe(*ethernetModule, EthernetSendEvent::TYPE);
        eventBus->subscribe(*ip4Module, IP4SendEvent::TYPE);

    }

    NetworkService::~NetworkService() {
        //TODO: Synchronisierung nötig?
        eventBus->unsubscribe(*ip4Module, IP4SendEvent::TYPE);
        eventBus->unsubscribe(*ethernetModule, EthernetSendEvent::TYPE);

        eventBus->unsubscribe(packetHandler, ReceiveEvent::TYPE);
//        eventBus->unsubscribe(*ethernetModule, EthernetReceiveEvent::TYPE);
//        eventBus->unsubscribe(*ip4Module, ARPReceiveEvent::TYPE);
//        eventBus->unsubscribe(*ip4Module, IP4ReceiveEvent::TYPE);
//
        delete ip4Module;
        delete ethernetModule;
    }

    uint32_t NetworkService::getDeviceCount() {
        return drivers.size();
    }

    NetworkDevice &NetworkService::getDriver(uint8_t index) {
        return *drivers.get(index);
    }

    void NetworkService::removeDevice(uint8_t index) {
        if (index >= getDeviceCount()) {
            return;
        }
        NetworkDevice *selectedDriver = drivers.get(index);
        ip4Module->unregisterDevice(ethernetModule->getEthernetDevice(selectedDriver));
        ethernetModule->unregisterNetworkDevice(selectedDriver);
        drivers.remove(selectedDriver);
    }

    void NetworkService::registerDevice(String *identifier, NetworkDevice &driver) {
        if (identifier == nullptr) {
            log.error("Given identifier was null, not registering it");
            return;
        }
        ethernetModule->registerNetworkDevice(identifier, &driver);
        drivers.add(&driver);
    }

    void NetworkService::registerDevice(NetworkDevice &driver) {
        ethernetModule->registerNetworkDevice(&driver);
        drivers.add(&driver);
    }

    void NetworkService::collectLinkAttributes(Util::ArrayList<String> *strings) {
        this->ethernetModule->collectEthernetDeviceAttributes(strings);
    }

    void NetworkService::collectInterfaceAttributes(Util::ArrayList<String> *strings) {
        this->ip4Module->collectIP4InterfaceAttributes(strings);
    }

    //We don't know IP4Addresses at system startup, so we need to set it later via this method here
    void NetworkService::assignIP4Address(String *identifier, IP4Address *ip4Address, IP4Netmask *ip4Netmask) {
        if (identifier == nullptr || ip4Address == nullptr || ip4Netmask == nullptr) {
            log.error("At least one of given attributes were null, not assigning IP4 address");
            return;
        }
        EthernetDevice *selected = this->ethernetModule->getEthernetDevice(identifier);
        if (selected == nullptr) {
            log.error("No ethernet device exists for given identifier, not assigning IP4 address");
            return;
        }
        this->ip4Module->registerDevice(selected, ip4Address, ip4Netmask);
    }
}
