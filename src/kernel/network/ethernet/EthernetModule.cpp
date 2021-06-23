//
// Created by hannes on 15.05.21.
//

#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include "EthernetModule.h"

namespace Kernel {
    //Private method!
    void EthernetModule::deleteSendBuffer(const EthernetDevice *ethernetDevice) {
        if (ethernetDevice == nullptr) {
            return;
        }
        if (ethernetDevice->getPhysicalBufferAddress() != nullptr) {
            //Free mapped IO if physical interface
            systemManagement->freeIO(ethernetDevice->getSendBuffer());
        } else {
            //Simply delete allocated buffer if virtual interface
            delete ethernetDevice->getSendBuffer();
        }
    }

    //Private method!
    bool EthernetModule::isForUsOrBroadcast(EthernetHeader *ethernetHeader) {
        if (ethernetHeader->destinationIs(broadcastAddress)) {
            return true;
        }
        if (ethernetDevices == nullptr || accessLock == nullptr) {
            log.error("Internal device list or accessLock was null, not checking if frame is for us");
            return false;
        }
        accessLock->acquire();
        for (EthernetDevice *current:*ethernetDevices) {
            if (ethernetHeader->destinationIs(current->getAddress())) {
                accessLock->release();
                return true;
            }
        }
        accessLock->release();
        return false;
    }

    EthernetModule::EthernetModule(Management *systemManagement, NetworkEventBus *eventBus,
                                   EthernetDeviceIdentifier *loopbackIdentifier) {
        this->eventBus = eventBus;
        this->loopbackIdentifier = loopbackIdentifier;
        this->systemManagement = systemManagement;

        broadcastAddress = EthernetAddress::buildBroadcastAddress();
        ethernetDevices = new Util::ArrayList<EthernetDevice *>();
        accessLock = new Spinlock();
        accessLock->release();
    }

    EthernetModule::~EthernetModule() {
        delete accessLock;
        delete broadcastAddress;

        if (ethernetDevices == nullptr) {
            return;
        }
        EthernetDevice *toDelete;
        for (size_t i = 0; i < ethernetDevices->size(); i++) {
            //Deleting while iterating is always dangerous
            //-> execute get() and remove() separately!
            toDelete = ethernetDevices->get(i);
            ethernetDevices->remove(i);
            i--;
            deleteSendBuffer(toDelete);
            delete toDelete;
        }
        delete ethernetDevices;
    }

    void EthernetModule::registerNetworkDevice(NetworkDevice *networkDevice) {
        if (networkDevice == nullptr) {
            log.error("Given network device was null, not registering it");
            return;
        }
        this->registerNetworkDevice(
                new EthernetDeviceIdentifier(deviceCounter),
                networkDevice
        );
        deviceCounter++;
    }

    uint8_t EthernetModule::registerNetworkDevice(EthernetDeviceIdentifier *identifier, NetworkDevice *networkDevice) {
        if (identifier == nullptr || networkDevice == nullptr) {
            log.error("Given identifier or network device was null, not registering it");
            return 1;
        }
        if (ethernetDevices == nullptr || accessLock == nullptr) {
            log.error("Internal list or accessLock was null, not registering network device");
            return 1;
        }
        accessLock->acquire();
        //Return if an ethernet device connected to the same network device could be found
        for (EthernetDevice *currentDevice:*ethernetDevices) {
            if (currentDevice->connectedTo(networkDevice)) {
                log.error("Given network device already registered, ignoring it");
                accessLock->release();
                return 1;
            }
        }

        if (identifier->equals(loopbackIdentifier)) {
            auto *sendBuffer = new uint8_t[EthernetHeader::getMaximumFrameLength()];
            memset(sendBuffer, 0, EthernetHeader::getMaximumFrameLength());
            this->ethernetDevices->add(new EthernetDevice(sendBuffer, identifier, networkDevice));
            accessLock->release();
            return 0;
        }

        auto *sendBuffer = (uint8_t *) this->systemManagement->mapIO(EthernetHeader::getMaximumFrameLength());
        memset(sendBuffer, 0, EthernetHeader::getMaximumFrameLength());

        auto *physicalBufferAddress = this->systemManagement->getPhysicalAddress(sendBuffer);
        auto *toAdd = new EthernetDevice(sendBuffer, physicalBufferAddress, identifier, networkDevice);

        this->ethernetDevices->add(toAdd);
        accessLock->release();

        return 0;
    }

    uint8_t EthernetModule::unregisterNetworkDevice(NetworkDevice *networkDevice) {
        EthernetDevice *connectedDevice = getEthernetDevice(networkDevice);
        if (connectedDevice == nullptr) {
            log.error("No connected ethernet device could be found, not unregistering network device");
            return 1;
        }
        if (ethernetDevices == nullptr || accessLock == nullptr) {
            log.error("Internal list or accessLock was null, not unregistering network device");
            return 1;
        }
        accessLock->acquire();
        EthernetDevice *toDelete;
        for (size_t i = 0; i < ethernetDevices->size(); i++) {
            if (ethernetDevices->get(i)->connectedTo(networkDevice)) {
                toDelete = ethernetDevices->get(i);
                ethernetDevices->remove(i);
                accessLock->release();
                deleteSendBuffer(toDelete);
                delete toDelete;
                return 0;
            }
        }
        accessLock->release();
        log.error("Given network device was not registered, ignoring");
        return 1;
    }

    uint8_t EthernetModule::collectEthernetDeviceAttributes(Util::ArrayList<String> *strings) {
        if (strings == nullptr || ethernetDevices == nullptr || accessLock == nullptr) {
            log.error("Given String list, internal list or accessLock was null,"
                      "not collecting device attributes");
            return 1;
        }
        accessLock->acquire();
        for (EthernetDevice *currentDevice:*ethernetDevices) {
            strings->add(currentDevice->asString());
        }
        accessLock->release();
        return 0;
    }

//Get ethernet device via identifier
    EthernetDevice *EthernetModule::getEthernetDevice(EthernetDeviceIdentifier *identifier) {
        if (ethernetDevices == nullptr || accessLock == nullptr) {
            log.error("Internal list or accessLock was null, not searching ethernet device");
            return nullptr;
        }
        accessLock->acquire();
        for (EthernetDevice *currentDevice:*ethernetDevices) {
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
        if (ethernetDevices == nullptr || accessLock == nullptr) {
            log.error("Internal list or accessLock was null, not searching ethernet device");
            return nullptr;
        }
        accessLock->acquire();
        for (EthernetDevice *currentDevice:*ethernetDevices) {
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
            EthernetDevice *outDevice = ((EthernetSendEvent &) event).getOutDevice();
            EthernetFrame *outFrame = ((EthernetSendEvent &) event).getEthernetFrame();

            if (outDevice == nullptr) {
                log.error("Outgoing device was null, discarding frame");
                //delete on NULL objects simply does nothing
                delete outFrame;
                return;
            }
            if (outFrame == nullptr) {
                log.error("Outgoing frame was null, ignoring");
                return;
            }
            if (outFrame->getLengthInBytes() == 0) {
                log.error("Outgoing frame was empty, discarding frame");
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
                    eventBus->publish(new IP4ReceiveEvent(input));
                    break;
                }
                case EthernetDataPart::EtherType::ARP: {
                    //send input to ARPModule via EventBus for further processing
                    eventBus->publish(new ARPReceiveEvent(input));
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