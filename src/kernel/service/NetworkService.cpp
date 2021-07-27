

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
        accessLock = new Spinlock();
        accessLock->release();

        for (auto &buffer : buffers) {
            buffer = nullptr;
        }

        packetHandler = new PacketHandler(eventBus);
        ethernetModule = new EthernetModule(eventBus);
        ip4Module = new IP4Module(eventBus);
        icmp4Module = new ICMP4Module(eventBus);
        udp4Module = new UDP4Module(eventBus);

        //Setup Loopback with 127.0.0.1/8 and inMemory send buffer
        loopbackBuffer = new uint8_t[EthernetHeader::maximumFrameLength()];
        memset(loopbackBuffer, 0, EthernetHeader::maximumFrameLength());

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
        uint32_t deviceCount;

        accessLock->acquire();
        deviceCount = drivers.size();
        accessLock->release();

        return deviceCount;
    }

    NetworkDevice &NetworkService::getDriver(uint8_t index) {
        NetworkDevice *driver;

        accessLock->acquire();
        driver = drivers.get(index);
        accessLock->release();

        return *driver;
    }

    void NetworkService::removeDevice(uint8_t index) {
        accessLock->acquire();
        if (index >= drivers.size()) {
            accessLock->release();
            return;
        }
        NetworkDevice *selectedDriver = drivers.get(index);
        ip4Module->unregisterDevice(ethernetModule->getEthernetDevice(selectedDriver));
        ethernetModule->unregisterNetworkDevice(selectedDriver);
        drivers.remove(selectedDriver);

        //Index 0 is for loopback!
        if (index > 0 && buffers[index - 1] != nullptr) {
            management->freeIO(buffers[index - 1]);
            buffers[index - 1] = nullptr;
        }
        accessLock->release();
    }

    void NetworkService::registerDevice(NetworkDevice &driver) {
        accessLock->acquire();
        size_t targetPosition = deviceCounter;
        if (deviceCounter == MAX_DEVICE_COUNT) {
            size_t i;
            for (i = 0; i < MAX_DEVICE_COUNT; i++) {
                if (buffers[i] == nullptr) {
                    targetPosition = i;
                }
            }
            //If no free position found
            if (i == MAX_DEVICE_COUNT) {
                log.error("Max device count reached, not registering new device");
                accessLock->release();
                return;
            }
        }
        buffers[targetPosition] = (uint8_t *) management->mapIO(EthernetHeader::maximumFrameLength());
        memset(buffers[targetPosition], 0, EthernetHeader::maximumFrameLength());

        auto *physicalBufferAddress = management->getPhysicalAddress(buffers[targetPosition]);

        String identifier = String::format("eth%d", targetPosition);
        ethernetModule
                ->registerNetworkDevice(identifier, &driver, buffers[targetPosition], physicalBufferAddress);
        drivers.add(&driver);
        deviceCounter++;
        accessLock->release();
    }

    uint8_t NetworkService::collectLinkAttributes(Util::ArrayList<String> *strings) {
        uint8_t result;

        accessLock->acquire();
        result = ethernetModule->collectEthernetDeviceAttributes(strings);
        accessLock->release();

        return result;
    }

    uint8_t NetworkService::collectInterfaceAttributes(Util::ArrayList<String> *strings) {
        uint8_t result;

        accessLock->acquire();
        result = ip4Module->collectIP4InterfaceAttributes(strings);
        accessLock->release();

        return result;
    }

    uint8_t NetworkService::collectRouteAttributes(Util::ArrayList<String> *strings) {
        uint8_t result;

        accessLock->acquire();
        result = ip4Module->collectIP4RouteAttributes(strings);
        accessLock->release();

        return result;
    }

    uint8_t NetworkService::collectARPTables(Util::ArrayList<String> *strings) {
        uint8_t result;

        accessLock->acquire();
        result = ip4Module->collectARPTables(strings);
        accessLock->release();

        return result;
    }

    //We don't know IP4Addresses at system startup, so we need to set it later via this method here
    uint8_t NetworkService::assignIP4Address(const String &identifier, IP4Address *ip4Address, IP4Netmask *ip4Netmask) {
        if (identifier == nullptr || ip4Address == nullptr || ip4Netmask == nullptr) {
            log.error("At least one of given attributes were null, not assigning IP4 address");
            return 1;
        }
        accessLock->acquire();
        EthernetDevice *selected = ethernetModule->getEthernetDevice(identifier);
        if (selected == nullptr) {
            log.error("No ethernet device exists for %s, not assigning IP4 address", (char *) identifier);
            accessLock->release();
            return 1;
        }
        if (ip4Module->registerDevice(selected, ip4Address, ip4Netmask)) {
            log.error("Registering device %s failed", (char *) identifier);
            accessLock->release();
            return 1;
        }
        accessLock->release();
        return 0;
    }

    uint8_t NetworkService::unAssignIP4Address(const String &identifier) {
        if (identifier == nullptr) {
            log.error("Given identifier was null, not unAssigning IP4 address");
            return 1;
        }
        accessLock->acquire();
        EthernetDevice *selected = ethernetModule->getEthernetDevice(identifier);
        if (selected == nullptr) {
            log.error("No ethernet device exists for %s, not unAssigning IP4 address", (char *) identifier);
            accessLock->release();
            return 1;
        }
        uint8_t result = ip4Module->unregisterDevice(selected);
        accessLock->release();
        return result;
    }

    UDP4SocketController *NetworkService::createSocketController(uint16_t bufferSize) {
        //No sync necessary here
        if (bufferSize == 0) {
            bufferSize = 1;
        }
        return new UDP4SocketController(eventBus, bufferSize);
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
        accessLock->acquire();
        uint8_t result = udp4Module->registerControllerFor(listeningPort, controller);
        accessLock->release();
        return result;
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
        accessLock->acquire();
        uint8_t result = udp4Module->registerControllerFor(listeningPortTarget, controller);
        accessLock->release();
        return result;
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
        accessLock->acquire();
        uint8_t result = udp4Module->unregisterControllerFor(destinationPort);
        accessLock->release();
        return result;
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
        accessLock->acquire();
        uint8_t result = ip4Module->setDefaultRoute(gatewayAddress, outDevice);
        accessLock->release();
        return result;
    }

    uint8_t NetworkService::removeDefaultRoute() {
        if (ip4Module == nullptr) {
            log.error("IP4Module not initialized, not removing default route");
            return 1;
        }
        accessLock->acquire();
        uint8_t result = ip4Module->removeDefaultRoute();
        accessLock->release();
        return result;
    }
}
