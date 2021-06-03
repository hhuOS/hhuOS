//
// Created by hannes on 25.05.21.
//

#include <kernel/event/network/EthernetSendEvent.h>
#include "IP4Interface.h"

IP4Interface::IP4Interface(Kernel::NetworkEventBus *eventBus, EthernetDevice *ethernetDevice, IP4Address *ip4Address,
                           IP4Netmask *ip4Netmask) {
    this->eventBus = eventBus;
    this->arpModule = new ARPModule();
    this->ethernetDevice = ethernetDevice;
    this->ip4Address = ip4Address;
    this->ip4Netmask = ip4Netmask;

    this->arpModule->addEntry(ip4Address, ethernetDevice->getAddress());
}

IP4Interface::~IP4Interface() {
    delete this->arpModule;
}

void IP4Interface::sendIP4Datagram(IP4Address *receiver, IP4Datagram *ip4Datagram) {
    ip4Datagram->setSourceAddress(this->ip4Address);
    this->eventBus->publish(
            new Kernel::EthernetSendEvent(
                    this->ethernetDevice,
                    this->arpModule->initEthernetFrame(receiver, ip4Datagram)
            )
    );
}

IP4Address *IP4Interface::getIp4Address() const {
    return ip4Address;
}

IP4Netmask *IP4Interface::getIp4Netmask() const {
    return ip4Netmask;
}

bool IP4Interface::equals(IP4Interface *compare) {
    return this->ethernetDevice == compare->ethernetDevice;
}

IP4Address *IP4Interface::getNetAddress() const {
    return this->ip4Netmask->extractNetPart(this->getIp4Address());
}

String IP4Interface::asString() {
    return this->ethernetDevice->asString() + ",\nIP4Address: " + this->ip4Address->asString() + ",\nIP4Netmask: " +
           this->ip4Netmask->asString();
}
