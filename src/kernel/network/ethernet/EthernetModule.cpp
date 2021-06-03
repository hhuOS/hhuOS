//
// Created by hannes on 15.05.21.
//

#include <kernel/core/System.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>

#include "EthernetModule.h"
#include "EthernetDevice.h"


Kernel::EthernetModule::EthernetModule(NetworkEventBus *eventBus) {
    this->deviceCounter = 0;
    this->eventBus = eventBus;
    this->ethernetDevices = new Util::HashMap<String *, EthernetDevice *>();
}

void Kernel::EthernetModule::registerNetworkDevice(NetworkDevice *networkDevice) {
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

void Kernel::EthernetModule::registerNetworkDevice(String *identifier, NetworkDevice *networkDevice) {
    if (identifier == nullptr) {
        log.error("Given identifier was null, not registering it");
        return;
    }
    if (networkDevice == nullptr) {
        log.error("Given network device was null, not registering it");
        return;
    }
    //Return if an ethernet device connected to the same network device could be found
    if (ethernetDevices->containsKey(identifier)) {
        log.error("Given identifier already exists, ignoring it");
        return;
    }
    //Add a new connected ethernet device if no duplicate found
    this->ethernetDevices->put(identifier, new EthernetDevice(identifier, networkDevice));
}

void Kernel::EthernetModule::unregisterNetworkDevice(NetworkDevice *networkDevice) {
    EthernetDevice *connectedDevice = getEthernetDevice(networkDevice);
    if (connectedDevice != nullptr) {
        this->ethernetDevices->remove(connectedDevice->getIdentifier());
    }
}

void Kernel::EthernetModule::collectEthernetDeviceAttributes(Util::ArrayList<String> *strings) {
    for (String *currentKey:ethernetDevices->keySet()) {
        strings->add(getEthernetDevice(currentKey)->asString());
    }
}

//Get ethernet device via identifier
EthernetDevice *Kernel::EthernetModule::getEthernetDevice(String *identifier) {
    if (ethernetDevices->containsKey(identifier)) {
        return ethernetDevices->get(identifier);
    }
    return nullptr;
}

//Get ethernet device via network device it's connected to
EthernetDevice *Kernel::EthernetModule::getEthernetDevice(NetworkDevice *networkDevice) {
    for (String *current:this->ethernetDevices->keySet()) {
        if (getEthernetDevice(current)->connectedTo(networkDevice)) {
            return getEthernetDevice(current);
        }
    }
    return nullptr;
}

void Kernel::EthernetModule::onEvent(const Kernel::Event &event) {
    if ((event.getType() == EthernetSendEvent::TYPE)) {
        auto sendEvent = ((EthernetSendEvent &) event);
        EthernetDevice *outDevice = sendEvent.getOutDevice();
        EthernetFrame *outFrame = sendEvent.getEthernetFrame();

        if (outFrame != nullptr && outDevice != nullptr) {
            outDevice->sendEthernetFrame(outFrame);
        }
        return;
    }
    if ((event.getType() == EthernetReceiveEvent::TYPE)) {
        EthernetFrame *inFrame = ((EthernetReceiveEvent &) event).getEthernetFrame();
        if (inFrame->parseInput()) {
            log.error("Parsing of incoming EthernetFrame failed, discarding");
            delete inFrame;
            return;
        }
        switch (inFrame->getEtherType()) {
            case EthernetDataPart::EtherType::IP4:
                eventBus->publish(
                        new Kernel::IP4ReceiveEvent(
                                inFrame->buildIP4DatagramWithInput()
                        )
                );
                return;
            case EthernetDataPart::EtherType::ARP:
                eventBus->publish(
                        new Kernel::ARPReceiveEvent(
                                inFrame->buildARPMessageWithInput()
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
