

#include <kernel/core/System.h>
#include <device/network/loopback/Loopback.h>
#include <kernel/event/network/UDP4ReceiveEvent.h>
#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/event/network/ICMP4SendEvent.h>
#include <kernel/event/network/UDP4SendEvent.h>
#include "NetworkService.h"

namespace Kernel {

    NetworkService::NetworkService() {
        eventBus = System::getService<EventBus>();
        management = &Management::getInstance();

        for (auto &buffer : buffers) {
            buffer = nullptr;
        }

        packetHandler = new PacketHandler(eventBus);
        ethernetModule = new EthernetModule(eventBus);
        ip4Module = new IP4Module(eventBus);
        icmp4Module = new ICMP4Module(eventBus);
        udp4Module = new UDP4Module(eventBus);

        //Setup Loopback with 127.0.0.1/8 and inMemory send buffer
        loopbackBuffer = new uint8_t[EthernetHeader::getMaximumFrameLength()];
        memset(loopbackBuffer, 0, EthernetHeader::getMaximumFrameLength());

        auto loopbackDevice = new Loopback(eventBus);
        drivers.add(loopbackDevice);
        ethernetModule->
                registerNetworkDevice("lo", loopbackDevice, loopbackBuffer, nullptr);
        assignIP4Address(
                "lo",
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

        delete udp4Module;
        udp4Module = nullptr;

        delete icmp4Module;
        icmp4Module = nullptr;

        delete ip4Module;
        ip4Module = nullptr;

        delete ethernetModule;
        ethernetModule = nullptr;

        delete packetHandler;
        packetHandler = nullptr;

        delete loopbackBuffer;
        for (auto &currentBuffer : buffers) {
            if (currentBuffer != nullptr) {
                management->freeIO(currentBuffer);
            }
        }
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

        if (index < deviceCounter && buffers[index] != nullptr) {
            management->freeIO(buffers[index]);
        }
    }

    void NetworkService::registerDevice(NetworkDevice &driver) {
        buffers[deviceCounter] = (uint8_t *) management->mapIO(EthernetHeader::getMaximumFrameLength());
        memset(buffers[deviceCounter], 0, EthernetHeader::getMaximumFrameLength());

        auto *physicalBufferAddress = management->getPhysicalAddress(buffers[deviceCounter]);

        String identifier = String::format("eth%d", deviceCounter);
        ethernetModule
        ->registerNetworkDevice(identifier, &driver, buffers[deviceCounter], physicalBufferAddress);
        drivers.add(&driver);
        deviceCounter++;
    }

    uint8_t NetworkService::collectLinkAttributes(Util::ArrayList<String> *strings) {
        return this->ethernetModule->collectEthernetDeviceAttributes(strings);
    }

    uint8_t NetworkService::collectInterfaceAttributes(Util::ArrayList<String> *strings) {
        return this->ip4Module->collectIP4InterfaceAttributes(strings);
    }

    uint8_t NetworkService::collectRouteAttributes(Util::ArrayList<String> *strings) {
        return this->ip4Module->collectIP4RouteAttributes(strings);
    }

    //We don't know IP4Addresses at system startup, so we need to set it later via this method here
    uint8_t NetworkService::assignIP4Address(const String &identifier, IP4Address *ip4Address, IP4Netmask *ip4Netmask) {
        if (identifier == nullptr || ip4Address == nullptr || ip4Netmask == nullptr) {
            log.error("At least one of given attributes were null, not assigning IP4 address");
            return 1;
        }
        EthernetDevice *selected = this->ethernetModule->getEthernetDevice(identifier);
        if (selected == nullptr) {
            log.error("No ethernet device exists for %s, not assigning IP4 address", (char *) identifier);
            return 1;
        }
        if (this->ip4Module->registerDevice(selected, ip4Address, ip4Netmask)) {
            log.error("Registering device %s failed", (char *) identifier);
            return 1;
        }
        return 0;
    }

    uint8_t NetworkService::unAssignIP4Address(const String &identifier) {
        if (identifier == nullptr) {
            log.error("Given identifier was null, not unAssigning IP4 address");
            return 1;
        }
        EthernetDevice *selected = this->ethernetModule->getEthernetDevice(identifier);
        if (selected == nullptr) {
            log.error("No ethernet device exists for %s, not unAssigning IP4 address", (char *) identifier);
            return 1;
        }
        return this->ip4Module->unregisterDevice(selected);
    }

    UDP4SocketController *NetworkService::createSocketController(size_t bufferSize) {
        if (bufferSize == 0) {
            bufferSize = 1;
        }
        return new UDP4SocketController(this->eventBus, bufferSize);
    }

    uint8_t NetworkService::registerSocketController(uint16_t listeningPort, UDP4SocketController *controller) {
        if (listeningPort == 0) {
            log.error("Listening port was zero, not registering");
            return 1;
        }
        if (controller == nullptr) {
            log.error("Controller was null, not registering");
            return 1;
        }
        if (udp4Module == nullptr) {
            log.error("UDP4Module not initialized, not registering controller");
            return 1;
        }
        return udp4Module->registerControllerFor(listeningPort, controller);
    }

    uint8_t NetworkService::registerSocketController(uint16_t *listeningPortTarget, UDP4SocketController *controller) {
        if (listeningPortTarget == nullptr) {
            log.error("Listening port target was null, not registering");
            return 1;
        }
        if (controller == nullptr) {
            log.error("Controller was null, not registering");
            return 1;
        }
        if (udp4Module == nullptr) {
            log.error("UDP4Module not initialized, not registering controller");
            return 1;
        }
        return udp4Module->registerControllerFor(listeningPortTarget, controller);
    }

    uint8_t NetworkService::unregisterSocketController(uint16_t destinationPort) {
        if (destinationPort == 0) {
            log.error("Destination port was zero, not unregistering controller");
            return 1;
        }
        if (udp4Module == nullptr) {
            log.error("UDP4Module not initialized, not unregistering controller");
            return 1;
        }
        return udp4Module->unregisterControllerFor(destinationPort);
    }

    uint8_t NetworkService::setDefaultRoute(IP4Address *gatewayAddress, const String &outDevice) {
        if (gatewayAddress == nullptr || outDevice == nullptr) {
            log.error("Gateway address or out device was null, not setting default route");
            return 1;
        }
        if (ip4Module == nullptr) {
            log.error("IP4Module not initialized, not setting default route");
            return 1;
        }
        return ip4Module->setDefaultRoute(gatewayAddress, outDevice);
    }

    uint8_t NetworkService::removeDefaultRoute() {
        if (ip4Module == nullptr) {
            log.error("IP4Module not initialized, not removing default route");
            return 1;
        }
        return ip4Module->removeDefaultRoute();
    }
}
