//
// Created by hannes on 25.05.21.
//

#include <kernel/network/internet/arp/ARPRequest.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include "IP4Interface.h"
#include "IP4Datagram.h"

IP4Interface::IP4Interface(Kernel::EventBus *eventBus, EthernetDevice *ethernetDevice) {
    this->eventBus = eventBus;
    this->arpModule = new ARPModule();
    this->ethernetDevice = ethernetDevice;
    this->ip4Address = new IP4Address(0, 0, 0, 0);
    this->ip4Netmask = new IP4Netmask(0, 0, 0, 0);
}

void IP4Interface::sendIP4Datagram(IP4Address *receiver, IP4Datagram *ip4Datagram) {
    EthernetAddress *destinationEthernetAddress = arpModule->resolveIP4(receiver);
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
