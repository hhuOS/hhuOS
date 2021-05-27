//
// Created by hannes on 15.05.21.
//

#include <kernel/core/System.h>
#include <kernel/service/EventBus.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include <kernel/network/internet/IP4Datagram.h>

#include "EthernetModule.h"
#include "EthernetDevice.h"


Kernel::EthernetModule::EthernetModule(Kernel::EventBus *eventBus) {
    this->deviceCounter=0;
    this->eventBus=eventBus;
    this->ethernetDevices = new Util::HashMap<String, EthernetDevice *>();
}

void Kernel::EthernetModule::registerNetworkDevice(NetworkDevice *networkDevice) {
    this->registerNetworkDevice(
            String::format("eth%d",deviceCounter),
            networkDevice
            );
    deviceCounter++;
}

void Kernel::EthernetModule::registerNetworkDevice(const String& identifier, NetworkDevice *networkDevice) {
    //Return if an ethernet device connected to the same network device could be found
    if(ethernetDevices->containsKey(identifier)){
        return;
    }
    //Add a new connected ethernet device if no duplicate found
    this->ethernetDevices->put(identifier, new EthernetDevice(identifier, networkDevice));
}

void Kernel::EthernetModule::unregisterNetworkDevice(NetworkDevice *networkDevice) {
    EthernetDevice *connectedDevice = getEthernetDevice(networkDevice);
    if(connectedDevice!= nullptr) {
        this->ethernetDevices->remove(connectedDevice->getIdentifier());
        delete connectedDevice;
    }
}

void Kernel::EthernetModule::onEvent(const Kernel::Event &event) {
    if (event.getType() == EthernetSendEvent::TYPE) {
        log.info("EthernetSendEvent with EthernetFrame received");

        auto sendEvent = ((EthernetSendEvent &) event);
        EthernetDevice *outDevice = sendEvent.getOutDevice();
        EthernetFrame *outFrame = sendEvent.getEthernetFrame();

        if (outFrame != nullptr && outDevice != nullptr) {
            outDevice->sendEthernetFrame(outFrame);
        }

        return;
    }
    if (event.getType() == EthernetReceiveEvent::TYPE) {
        auto receiveEvent = ((EthernetReceiveEvent &) event);
        EthernetFrame *inFrame = receiveEvent.getEthernetFrame();

        switch (inFrame->getEtherType()) {
            case EtherType::IP4:
                eventBus->publish(
                        Util::SmartPointer<Kernel::Event>(
                                new Kernel::IP4ReceiveEvent(
                                        new IP4Datagram(inFrame->getDataPart())
                                )
                        )
                );
                return;
            case EtherType::ARP:
                eventBus->publish(
                        Util::SmartPointer<Kernel::Event>(
                                new Kernel::ARPReceiveEvent(
                                        new ARPResponse(inFrame->getDataPart())
                                )
                        )
                );
                return;
            case EtherType::IP6:
                break;
        }
    }
}

void Kernel::EthernetModule::collectEthernetDeviceAttributes(Util::ArrayList<String> *strings) {
    for(const String& currentKey:ethernetDevices->keySet()){
        strings->add(getEthernetDevice(currentKey)->asString());
    }
}

//Get ethernet device via identifier
EthernetDevice *Kernel::EthernetModule::getEthernetDevice(const String& identifier) {
    if(ethernetDevices->containsKey(identifier)){
        return ethernetDevices->get(identifier);
    }
    return nullptr;
}

//Get ethernet device via network device it's connected to
EthernetDevice *Kernel::EthernetModule::getEthernetDevice(NetworkDevice *networkDevice) {
    for(const String& current:this->ethernetDevices->keySet()){
        if(getEthernetDevice(current)->connectedTo(networkDevice)){
            return getEthernetDevice(current);
        }
    }
    return nullptr;
}
