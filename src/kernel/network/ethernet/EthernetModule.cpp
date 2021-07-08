//
// Created by hannes on 15.05.21.
//

#include <kernel/network/DebugPrintout.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include "EthernetModule.h"

namespace Kernel {
    //Private method!
    bool EthernetModule::isForUsOrBroadcast(EthernetHeader *ethernetHeader) {
        if (ethernetHeader->destinationIs(broadcastAddress)) {
            return true;
        }
        if (devices == nullptr || accessLock == nullptr) {
            log.error("Internal device list or accessLock was null, not checking if frame is for us");
            return false;
        }
        accessLock->acquire();
        for (EthernetDevice *current:*devices) {
            if (current->isDestinationOf(ethernetHeader)) {
                accessLock->release();
                return true;
            }
        }
        accessLock->release();
        return false;
    }

    EthernetModule::EthernetModule(EventBus *eventBus) {
        this->eventBus = eventBus;

        broadcastAddress = EthernetAddress::buildBroadcastAddress();
        devices = new Util::ArrayList<EthernetDevice *>();
        accessLock = new Spinlock();
        accessLock->release();
    }

    EthernetModule::~EthernetModule() {
        delete accessLock;
        delete broadcastAddress;

        if (devices == nullptr) {
            return;
        }
        EthernetDevice *toDelete;
        for (uint32_t i = 0; i < devices->size(); i++) {
            //Deleting while iterating is always dangerous
            //-> execute get() and remove() separately!
            toDelete = devices->get(i);
            devices->remove(i);
            i--;
            delete toDelete;
        }
        delete devices;
    }

    uint8_t
    EthernetModule::registerNetworkDevice(const String &identifier, NetworkDevice *networkDevice, uint8_t *sendBuffer,
                                          void *physicalBufferAddress) {
        if (identifier.isEmpty() || networkDevice == nullptr || sendBuffer == nullptr) {
            log.error("Given identifier was empty, network device or send buffer was null, not registering");
            return 1;
        }
        if (devices == nullptr || accessLock == nullptr) {
            log.error("Internal list or accessLock was null, not registering network device");
            return 1;
        }
        accessLock->acquire();
        //Return if an ethernet device connected to the same network device could be found
        for (EthernetDevice *currentDevice:*devices) {
            if (currentDevice->connectedTo(networkDevice)) {
                log.error("Given network device %s already registered, not registering it again",
                          (char *) identifier);
                accessLock->release();
                return 1;
            }
        }

        this->devices->add(new EthernetDevice(identifier, networkDevice, sendBuffer, physicalBufferAddress));
        accessLock->release();

        return 0;
    }

    uint8_t EthernetModule::unregisterNetworkDevice(NetworkDevice *networkDevice) {
        EthernetDevice *connectedDevice = getEthernetDevice(networkDevice);
        if (connectedDevice == nullptr) {
            log.error("No connected ethernet device could be found, not unregistering network device");
            return 1;
        }
        if (devices == nullptr || accessLock == nullptr) {
            log.error("Internal list or accessLock was null, not unregistering network device");
            return 1;
        }
        accessLock->acquire();
        EthernetDevice *toDelete;
        for (uint32_t i = 0; i < devices->size(); i++) {
            if (devices->get(i)->connectedTo(networkDevice)) {
                toDelete = devices->get(i);
                devices->remove(i);
                accessLock->release();
                delete toDelete;
                return 0;
            }
        }
        accessLock->release();
        log.error("Given network device was not registered, ignoring");
        return 1;
    }

    uint8_t EthernetModule::collectEthernetDeviceAttributes(Util::ArrayList<String> *strings) {
        if (strings == nullptr || devices == nullptr || accessLock == nullptr) {
            log.error("Given String list, internal list or accessLock was null,"
                      "not collecting device attributes");
            return 1;
        }
        accessLock->acquire();
        for (EthernetDevice *currentDevice:*devices) {
            strings->add(currentDevice->asString());
        }
        accessLock->release();
        return 0;
    }

//Get ethernet device via identifier
    EthernetDevice *EthernetModule::getEthernetDevice(const String &identifier) {
        if (devices == nullptr || accessLock == nullptr) {
            log.error("Internal list or accessLock was null, not searching ethernet device");
            return nullptr;
        }
        accessLock->acquire();
        for (EthernetDevice *currentDevice:*devices) {
            if (currentDevice->sameIdentifierAs(identifier)) {
                accessLock->release();
                return currentDevice;
            }
        }
        accessLock->release();
        return nullptr;
    }

//Get ethernet device via network device it's connected to
    EthernetDevice *EthernetModule::getEthernetDevice(NetworkDevice *networkDevice) {
        if (devices == nullptr || accessLock == nullptr) {
            log.error("Internal list or accessLock was null, not searching ethernet device");
            return nullptr;
        }
        accessLock->acquire();
        for (EthernetDevice *currentDevice:*devices) {
            if (currentDevice->connectedTo(networkDevice)) {
                accessLock->release();
                return currentDevice;
            }
        }
        accessLock->release();
        return nullptr;
    }

    void EthernetModule::onEvent(const Event &event) {
        if ((event.getType() == EthernetSendEvent::TYPE)) {
            auto *outDevice = ((EthernetSendEvent &) event).getOutDevice();
            auto *targetHardwareAddress = ((EthernetSendEvent &) event).getTargetHardwareAddress();
            auto *dataPart = ((EthernetSendEvent &) event).getDataPart();

            if (dataPart == nullptr) {
                log.error("Outgoing data was null, ignoring");
                delete targetHardwareAddress;
                return;
            }
            if (targetHardwareAddress == nullptr) {
                log.error("Target hardware address was null, discarding outgoing data");
                targetHardwareAddress =
                        new EthernetAddress(0, 0, 0, 0, 0, 0);
                //EthernetFrame can cleanup EthernetDataParts internally
                //-> build one with given data and delete it then
                auto *cleanup = new EthernetFrame(targetHardwareAddress, dataPart);
                //targetHardwareAddress will be deleted internally
                delete cleanup;
                return;
            }
            auto *outFrame = new EthernetFrame(targetHardwareAddress, dataPart);
            if (outDevice == nullptr) {
                log.error("Outgoing device was null, discarding frame");
                //targetHardwareAddress will be deleted internally
                delete outFrame;
                return;
            }
            if (outFrame->getLengthInBytes() == 0) {
                log.error("Outgoing frame was empty, discarding frame");
                //targetHardwareAddress will be deleted internally
                delete outFrame;
                return;
            }
            if (outDevice->sendEthernetFrame(outFrame)) {
                log.error("Sending failed, see syslog for more details");
            }
            //NOTE: Any embedded data (like an IP4Datagram) will be deleted here as well
            delete outFrame;
            return;
        }
        if ((event.getType() == EthernetReceiveEvent::TYPE)) {
            auto *input = ((EthernetReceiveEvent &) event).getInput();
            if (input == nullptr) {
                log.error("Incoming input was null, ignoring");
                return;
            }
            auto *ethernetHeader = new EthernetHeader();
            if (ethernetHeader->parse(input)) {
                log.error("Parsing EthernetHeader failed, discarding");
                delete ethernetHeader;
                delete input;
                return;
            }
            if (!isForUsOrBroadcast(ethernetHeader)) {
                log.error("Incoming frame is not for us and not broadcast either, discarding");
                delete ethernetHeader;
                delete input;
                return;
            }
            switch (ethernetHeader->getEtherType()) {
                case EthernetDataPart::EtherType::IP4: {
                    //send input to IP4Module via EventBus for further processing
                    auto ip4receiveInputEvent =
                            Util::SmartPointer<Event>(new IP4ReceiveEvent(input));
                    eventBus->publish(ip4receiveInputEvent);
                    break;
                }
                case EthernetDataPart::EtherType::ARP: {
                    //send input to ARPModule via EventBus for further processing
                    auto arpReceiveInputEvent =
                            Util::SmartPointer<Event>(new ARPReceiveEvent(input));
                    eventBus->publish(arpReceiveInputEvent);
                    break;
                }
                default: {
                    log.error("EtherType of incoming EthernetFrame not supported, discarding data");
                    delete input;
                    break;
                }
            }
            //Input will be used in next module
            //-> no 'delete input' here
            delete ethernetHeader;
            return;
        }
    }
}