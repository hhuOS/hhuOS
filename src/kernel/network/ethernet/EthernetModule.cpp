//
// Created by hannes on 15.05.21.
//

#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/network/internet/IP4Header.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/network/arp/ARPMessage.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include <kernel/core/Management.h>
#include "EthernetModule.h"
#include "EthernetDeviceIdentifier.h"

namespace Kernel {
    EthernetModule::EthernetModule(Management *systemManagement, NetworkEventBus *eventBus,
                                   EthernetDeviceIdentifier *loopbackIdentifier) {
        this->eventBus = eventBus;
        this->loopbackIdentifier = loopbackIdentifier;
        this->systemManagement = systemManagement;
        ethernetDevices = new Util::ArrayList<EthernetDevice *>();
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

    void EthernetModule::registerNetworkDevice(EthernetDeviceIdentifier *identifier, NetworkDevice *networkDevice) {
        if (identifier == nullptr) {
            log.error("Given identifier was null, not registering it");
            return;
        }
        if (networkDevice == nullptr) {
            log.error("Given network device was null, not registering it");
            return;
        }
        if (ethernetDevices == nullptr) {
            log.error("Internal list of ethernet devices was null, not registering network device");
            return;
        }
        //Return if an ethernet device connected to the same network device could be found
        for (EthernetDevice *currentDevice:*ethernetDevices) {
            if (currentDevice->connectedTo(networkDevice)) {
                log.error("Given network device already registered, ignoring it");
                return;
            }
        }

        if(identifier->equals(loopbackIdentifier)){
            auto *sendBuffer = new uint8_t[EthernetHeader::getMaximumFrameLength()];
            this->ethernetDevices->add(new EthernetDevice(sendBuffer, identifier, networkDevice));
            return;
        }

        auto *sendBuffer = (uint8_t *)this->systemManagement->mapIO(EthernetHeader::getMaximumFrameLength());
        auto *physicalBufferAddress = this->systemManagement->getPhysicalAddress(sendBuffer);
        auto *toAdd = new EthernetDevice(sendBuffer, physicalBufferAddress, identifier, networkDevice);

        this->ethernetDevices->add(toAdd);
    }

    void EthernetModule::unregisterNetworkDevice(NetworkDevice *networkDevice) {
        EthernetDevice *connectedDevice = getEthernetDevice(networkDevice);
        if (connectedDevice == nullptr) {
            log.error("No connected ethernet device could be found, not unregistering network device");
            return;
        }
        if (ethernetDevices == nullptr) {
            log.error("Internal list of ethernet devices was null, not unregistering network device");
            return;
        }
        for (size_t i = 0; i < ethernetDevices->size(); i++) {
            if (ethernetDevices->get(i)->connectedTo(networkDevice)) {
                auto *toDelete = ethernetDevices->get(i);
                ethernetDevices->remove(i);
                if(toDelete->getPhysicalBufferAddress()!= nullptr){
                    //Free mapped IO if physical interface
                    this->systemManagement->freeIO(toDelete->getSendBuffer());
                } else{
                    //Simply delete allocated buffer if virtual interface
                    delete toDelete->getSendBuffer();
                }
                delete toDelete;
                break;
            }
        }

    }

    void EthernetModule::collectEthernetDeviceAttributes(Util::ArrayList<String> *strings) {
        if (ethernetDevices == nullptr ||
            strings == nullptr
                ) {
            return;
        }
        for (EthernetDevice *currentDevice:*ethernetDevices) {
            strings->add(currentDevice->asString());
        }
    }

//Get ethernet device via identifier
    EthernetDevice *EthernetModule::getEthernetDevice(EthernetDeviceIdentifier *identifier) {
        if (ethernetDevices == nullptr) {
            log.error("Internal list of ethernet devices was null, not searching ethernet device");
            return nullptr;
        }
        for (EthernetDevice *currentDevice:*ethernetDevices) {
            if (currentDevice->sameIdentifierAs(identifier)) {
                return currentDevice;
            }
        }
        return nullptr;
    }

//Get ethernet device via network device it's connected to
    EthernetDevice *EthernetModule::getEthernetDevice(NetworkDevice *networkDevice) {
        if (ethernetDevices == nullptr) {
            log.error("Internal list of ethernet devices was null, not searching ethernet device");
            return nullptr;
        }
        for (EthernetDevice *currentDevice:*ethernetDevices) {
            if (currentDevice->connectedTo(networkDevice)) {
                return currentDevice;
            }
        }
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
            //TODO: Check frame's Source-MAC if it's for us or at least a BROADCAST message
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