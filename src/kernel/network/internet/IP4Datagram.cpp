//
// Created by hannes on 14.05.21.
//

#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    this->ip4ProtocolType = ip4DataPart->getIP4ProtocolType();
    this->destinationAddress = destinationAddress;
    this->ip4DataPart = ip4DataPart;

    //First four bits are value "4" -> IPv4
    //Second four bits are value "5" -> 5 "lines" header length, 4 Bytes per line
    header.version_headerLength=0x45;

    //standard type of service, no priority etc.
    header.typeOfService=0;

    //header length is given in "lines" of 4 Bytes each
    //-> total size in Bytes is (header length)*4 + length of data part in Bytes
    header.totalLength=(header.version_headerLength - 0x40)*4 + ip4DataPart->getLengthInBytes();

    //fragmentation not used here, fragment parameters not set
    header.identification=0;
    header.flags_fragmentOffset=0;

    //solid default value for small LANs, can be set from constructor if necessary
    header.timeToLive=64;

    headerChecksum = new IP4HeaderChecksum();
}

IP4Datagram::IP4Datagram(EthernetDataPart *ethernetDataPart) {
//TODO: Implement parsing of parameters from given ethernetDataPart
}

IP4ProtocolType IP4Datagram::getIp4ProtocolType() const {
    return ip4ProtocolType;
}

IP4Address *IP4Datagram::getSourceAddress() const {
    return sourceAddress;
}

void IP4Datagram::setSourceAddress(IP4Address *sourceAddress) {
    IP4Datagram::sourceAddress = sourceAddress;
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    return destinationAddress;
}

IP4DataPart *IP4Datagram::getIp4DataPart() const {
    return ip4DataPart;
}

void *IP4Datagram::getMemoryAddress() {
    return nullptr;
}

uint16_t IP4Datagram::getLength() {
    return 0;
}

EtherType IP4Datagram::getEtherType() {
    return EtherType::IP4;
}
