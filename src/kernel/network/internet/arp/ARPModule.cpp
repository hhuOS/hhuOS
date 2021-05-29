//
// Created by hannes on 15.05.21.
//

#include "ARPModule.h"
#include "ARPRequest.h"

ARPModule::ARPModule() {
    arpTable = new Util::ArrayList<ARPEntry *>();
    broadcastAddress =
            new EthernetAddress(0xff,0xff,0xff,0xff,0xff,0xff);
}

EthernetAddress *ARPModule::resolveIP4(IP4Address *ip4Address) {
    for(ARPEntry *current:*arpTable){
        if(current->matches(ip4Address)){
            return current->getEthernetAddress();
        }
    }
    return nullptr;
}

void ARPModule::addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) {
    arpTable->add(new ARPEntry(ip4Address,ethernetAddress));
}

EthernetFrame *ARPModule::initEthernetFrame(IP4Address *receiver, IP4Datagram *ip4datagram) {
    EthernetAddress *destinationAddress = resolveIP4(receiver);
    if(destinationAddress == nullptr){
        return new EthernetFrame(broadcastAddress,new ARPRequest(receiver));
    }
    return new EthernetFrame(destinationAddress,ip4datagram);
}
