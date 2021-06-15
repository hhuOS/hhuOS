
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/event/network/ICMP4SendEvent.h>
#include <kernel/event/network/UDP4ReceiveEvent.h>
#include <kernel/event/network/UDP4SendEvent.h>
#include "kernel/core/System.h"
#include "NetworkService.h"

namespace Kernel {

    NetworkService::NetworkService() {
        loopbackIdentifier = new String("lo");
        eventBus = new NetworkEventBus(System::getService<EventBus>());

        packetHandler = new PacketHandler(eventBus);
        ethernetModule = new EthernetModule(eventBus);
        ip4Module = new IP4Module(eventBus);
        icmp4Module = new ICMP4Module(eventBus);
        udp4Module = new UDP4Module(eventBus);

        //Setup Loopback with 127.0.0.1/8
        registerDevice(loopbackIdentifier, *(new Loopback(eventBus)));
        assignIP4Address(
                loopbackIdentifier,
                new IP4Address(127, 0, 0, 1),
                new IP4Netmask(8)
        );

        eventBus->subscribe(*udp4Module, UDP4ReceiveEvent::TYPE);
        eventBus->subscribe(*icmp4Module, ICMP4ReceiveEvent::TYPE);
        eventBus->subscribe(*ip4Module, IP4ReceiveEvent::TYPE);
        eventBus->subscribe(*ip4Module, ARPReceiveEvent::TYPE);
        eventBus->subscribe(*ethernetModule, EthernetReceiveEvent::TYPE);
        eventBus->subscribe(*packetHandler, ReceiveEvent::TYPE);

        eventBus->subscribe(*ethernetModule, EthernetSendEvent::TYPE);
        eventBus->subscribe(*ip4Module, IP4SendEvent::TYPE);
        eventBus->subscribe(*icmp4Module, ICMP4SendEvent::TYPE);
        eventBus->subscribe(*udp4Module, UDP4SendEvent::TYPE);
    }

    NetworkService::~NetworkService() {
        eventBus->unsubscribe(*udp4Module, UDP4SendEvent::TYPE);
        eventBus->unsubscribe(*icmp4Module, ICMP4SendEvent::TYPE);
        eventBus->unsubscribe(*ip4Module, IP4SendEvent::TYPE);
        eventBus->unsubscribe(*ethernetModule, EthernetSendEvent::TYPE);

        eventBus->unsubscribe(*packetHandler, ReceiveEvent::TYPE);
        eventBus->unsubscribe(*ethernetModule, EthernetReceiveEvent::TYPE);
        eventBus->unsubscribe(*ip4Module, ARPReceiveEvent::TYPE);
        eventBus->unsubscribe(*ip4Module, IP4ReceiveEvent::TYPE);
        eventBus->unsubscribe(*icmp4Module, ICMP4ReceiveEvent::TYPE);
        eventBus->unsubscribe(*udp4Module, UDP4ReceiveEvent::TYPE);
//
        delete udp4Module;
        delete icmp4Module;
        delete ip4Module;
        delete ethernetModule;
        delete packetHandler;
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

    void NetworkService::collectRouteAttributes(Util::ArrayList<String> *strings) {
        this->ip4Module->collectIP4RouteAttributes(strings);
    }

    //TODO: Add error codes as return values!
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

    uint8_t NetworkService::createSocketController(UDP4Port *destinationPort, UDP4SocketController **targetAddress) {
        if(targetAddress == nullptr){
            log.error("Controller was null, not registering");
            return 1;
        }
        if(udp4Module== nullptr){
            log.error("UDP4Module not initialized, not registering controller");
            return 1;
        }
        *targetAddress= new UDP4SocketController(this->eventBus, this->bufferSize);
        return udp4Module->registerControllerFor(destinationPort, *targetAddress);
    }

    //uint8_t NetworkService::registerSocketController(TCP4SocketController *controller) ...

    uint8_t NetworkService::unregisterSocketController(UDP4Port *destinationPort) {
        if(destinationPort == nullptr){
            log.error("Destination port was null, not unregistering controller");
            return 1;
        }
        if(udp4Module== nullptr){
            log.error("UDP4Module not initialized, not unregistering controller");
            return 1;
        }
        return udp4Module->unregisterControllerFor(destinationPort);
    }
}
