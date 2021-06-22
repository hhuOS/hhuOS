//
// Created by hannes on 15.05.21.
//

#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/network/internet/IP4Header.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/network/arp/ARPMessage.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include "EthernetModule.h"

namespace Kernel {
    bool EthernetModule::isForUs(EthernetHeader *ethernetHeader) {
        if (ethernetHeader->destinationIs(broadcastAddress)) {
            return true;
        }
        accessLock->acquire();
        for (EthernetDevice *current:*ethernetDevices) {
            if(ethernetHeader->destinationIs(current->getAddress())){
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
        ethernetDevices = new Util::ArrayList<EthernetDevice *>();
        accessLock = new Spinlock();
        accessLock->release();
        broadcastAddress = EthernetAddress::buildBroadcastAddress();
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
                deleteSendBuffer(toDelete);
                delete toDelete;
                break;
            }
        }
        accessLock->release();
        //It's not an error if there's nothing to delete
        return 0;
    }

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
            auto *ethernetHeader = ((EthernetReceiveEvent &) event).getEthernetHeader();
            auto *input = ((EthernetReceiveEvent &) event).getInput();
            if (ethernetHeader == nullptr) {
                log.error("Incoming EthernetHeader was null, discarding input");
                delete input;
                return;
            }
            if (input == nullptr) {
                log.error("Incoming input was null, discarding EthernetHeader");
                delete ethernetHeader;
                return;
            }
            if (ethernetDevices == nullptr || accessLock == nullptr) {
                log.error("Internal list or accessLock was null, not searching ethernet device");
                delete ethernetHeader;
                delete input;
                return;
            }
            if(!isForUs(ethernetHeader)){
                log.error("Incoming frame is not broadcast and not for us either, discarding");
                delete ethernetHeader;
                delete input;
                return;
            }
            switch (ethernetHeader->getEtherType()) {
                case EthernetDataPart::EtherType::IP4: {
                    auto *ip4Header = new IP4Header();
                    if (ip4Header->parse(input)) {
                        log.error("Could not assemble IP4 header, discarding data");
                        //ip4Header is not part of inFrame here
                        //-> we need to delete it separately!
                        delete ip4Header;
                        delete input;
                        break;
                    }
                    //send input to next module via EventBus
                    eventBus->publish(new IP4ReceiveEvent(ip4Header, input));
                    break;
                }
                case EthernetDataPart::EtherType::ARP: {
                    auto *arpMessage = new ARPMessage();
                    if (arpMessage->parse(input)) {
                        log.error("Could not assemble ARP message, discarding data");
                        //arpMessage is not part of inFrame here
                        //-> we need to delete it separately!
                        delete arpMessage;
                        delete input;
                        break;
                    }
                    //Input has been parsed completely here, can be deleted
                    delete input;
                    //send input to next module via EventBus
                    eventBus->publish(new ARPReceiveEvent(arpMessage));
                    break;
                }
                default: {
                    log.error("EtherType of incoming EthernetFrame not supported, discarding data");
                    delete input;
                    break;
                }
            }
            //We are done here, cleanup memory
            //Input will be used in next module, so no delete here
            delete ethernetHeader;
        }
    }
}