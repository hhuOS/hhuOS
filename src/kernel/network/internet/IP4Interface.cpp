//
// Created by hannes on 25.05.21.
//

#include <kernel/network/internet/arp/ARPRequest.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include "IP4Interface.h"
#include "IP4Datagram.h"

IP4Interface::IP4Interface(Kernel::EventBus *eventBus, EthernetDevice *ethernetDevice, IP4Address *ip4Address,
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
    EthernetAddress *destinationEthernetAddress = arpModule->resolveIP4(receiver);
    ip4Datagram->setSourceAddress(this->ip4Address);

    EthernetFrame *outFrame;
    if (destinationEthernetAddress == nullptr) {
        outFrame = new EthernetFrame(destinationEthernetAddress, new ARPRequest(receiver));
        //TODO: Implement data structure for waiting IP4Datagrams
    } else {
        outFrame = new EthernetFrame(destinationEthernetAddress, ip4Datagram);
    }

    eventBus->publish(
            Util::SmartPointer<Kernel::Event>(
                    new Kernel::EthernetSendEvent(ethernetDevice, outFrame)
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
