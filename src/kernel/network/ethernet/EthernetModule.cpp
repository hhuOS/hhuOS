//
// Created by hannes on 15.05.21.
//

#include <kernel/core/System.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>

#include "EthernetModule.h"

namespace Kernel {
    EthernetModule::EthernetModule(NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
        ethernetDevices = new Util::HashMap<String *, EthernetDevice *>();
    }

    void EthernetModule::registerNetworkDevice(NetworkDevice *networkDevice) {
        if (networkDevice == nullptr) {
            log.error("Given network device was null, not registering it");
            return;
        }
        this->registerNetworkDevice(
                new String(String::format("eth%d", deviceCounter)),
                networkDevice
        );
        deviceCounter++;
    }

    void EthernetModule::registerNetworkDevice(String *identifier, NetworkDevice *networkDevice) {
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
        if (ethernetDevices->containsKey(identifier)) {
            log.info("Given identifier already exists, ignoring it");
            return;
        }
        //Add a new connected ethernet device if no duplicate found
        this->ethernetDevices->put(identifier, new EthernetDevice(identifier, networkDevice));
    }

    void EthernetModule::unregisterNetworkDevice(NetworkDevice *networkDevice) {
        EthernetDevice *connectedDevice = getEthernetDevice(networkDevice);
        if (connectedDevice == nullptr) {
            log.info("No connected ethernet device could be found, not unregistering network device");
            return;
        }
        if (ethernetDevices == nullptr) {
            log.error("Internal list of ethernet devices was null, not unregistering network device");
            return;
        }
        ethernetDevices->remove(connectedDevice->getIdentifier());
    }

    void EthernetModule::collectEthernetDeviceAttributes(Util::ArrayList<String> *strings) {
        if (ethernetDevices == nullptr ||
            strings == nullptr
                ) {
            return;
        }
        for (String *currentKey:ethernetDevices->keySet()) {
            strings->add(getEthernetDevice(currentKey)->asString());
        }
    }

//Get ethernet device via identifier
    EthernetDevice *EthernetModule::getEthernetDevice(String *identifier) {
        if (ethernetDevices == nullptr) {
            log.error("Internal list of ethernet devices was null, not searching ethernet device");
            return nullptr;
        }
        if (ethernetDevices->containsKey(identifier)) {
            return ethernetDevices->get(identifier);
        }
        return nullptr;
    }

//Get ethernet device via network device it's connected to
    EthernetDevice *EthernetModule::getEthernetDevice(NetworkDevice *networkDevice) {
        if (ethernetDevices == nullptr) {
            log.error("Internal list of ethernet devices was null, not searching ethernet device");
            return nullptr;
        }
        for (String *current:ethernetDevices->keySet()) {
            if (getEthernetDevice(current)->connectedTo(networkDevice)) {
                return getEthernetDevice(current);
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
            if (outFrame->getLengthInBytes()==0){
                log.error("Outgoing frame was empty, discarding frame");
                delete outFrame;
                return;
            }
            switch (outDevice->sendEthernetFrame(outFrame)) {
                case ETH_DELIVER_SUCCESS: {
                    break;
                }
                case ETH_FRAME_NULL: {
                    log.error("Outgoing frame was null, ignoring");
                    return;
                }
                case ETH_DEVICE_NULL: {
                    log.error("Outgoing device was null, discarding frame");
                    break;
                }
                case ETH_COPY_BYTEBLOCK_FAILED: {
                    log.error("Copy to byteBlock failed, discarding frame");
                    break;
                }
                case ETH_COPY_BYTEBLOCK_INCOMPLETE: {
                    log.error("Copy to byteBlock incomplete, discarding frame");
                    break;
                }
                case BYTEBLOCK_NETWORK_DEVICE_NULL: {
                    log.error("Network device in byteBlock was null, discarding frame");
                    break;
                }
                case BYTEBLOCK_BYTES_NULL: {
                    log.error("Internal bytes in byteBlock were null, discarding frame");
                    break;
                }
                default:
                    log.error("Sending failed with unknown error, discarding frame");
                    break;
            }
            //we are done here, delete outFrame no matter if sending worked or not
            //-> we still have our log entry if sending failed
            //NOTE: An embedded IP4Datagram will be deleted here
            delete outFrame;
            return;
        }
        if ((event.getType() == EthernetReceiveEvent::TYPE)) {
            EthernetFrame *inFrame = ((EthernetReceiveEvent &) event).getEthernetFrame();
            switch (inFrame->getEtherType()) {
                case EthernetDataPart::EtherType::IP4:
                    eventBus->publish(
                            new IP4ReceiveEvent(
                                    (IP4Datagram *) inFrame->getEthernetDataPart()
                            )
                    );
                    return;
                case EthernetDataPart::EtherType::ARP:
                    eventBus->publish(
                            new ARPReceiveEvent(
                                    (ARPMessage *) inFrame->getEthernetDataPart()
                            )
                    );
                    return;
                default:
                    log.info("EtherType of incoming EthernetFrame not supported, discarding");
                    delete inFrame;
                    return;
            }
        }
    }
}