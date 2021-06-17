//
// Created by hannes on 14.05.21.
//

#include <kernel/network/internet/icmp/messages/ICMP4EchoReply.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include <kernel/network/NetworkDefinitions.h>
#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    this->ip4DataPart = ip4DataPart;
    this->header = new IP4Header(destinationAddress, ip4DataPart);
}

IP4Datagram::~IP4Datagram() {
    delete header;
    //dataPart is null if this datagram is an incoming one!
    //-> deleting is only necessary in an outgoing datagram
    if (ip4DataPart == nullptr) {
        return;
    }
    switch (header->getIP4ProtocolType()) {
        case IP4DataPart::IP4ProtocolType::ICMP4: {
            auto *icmp4Message = (ICMP4Message *) ip4DataPart;
            switch (icmp4Message->getICMP4MessageType()) {
                case ICMP4Message::ICMP4MessageType::ECHO_REPLY:
                    delete (ICMP4EchoReply *) ip4DataPart;
                    break;
                case ICMP4Message::ICMP4MessageType::ECHO:
                    delete (ICMP4Echo *) ip4DataPart;
                    break;
                default:
                    break;
            }
            break;
        }
        case IP4DataPart::IP4ProtocolType::UDP: {
            delete (UDP4Datagram *) ip4DataPart;
            break;
        }
        default:
            break;
    }
}

IP4DataPart::IP4ProtocolType IP4Datagram::getIP4ProtocolType() const {
    return header->getIP4ProtocolType();
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    return header->getDestinationAddress();
}

void IP4Datagram::setSourceAddress(IP4Address *source) {
    header->setSourceAddress(source);
}

size_t IP4Datagram::getLengthInBytes() {
    return header->getTotalLength();
}

EthernetDataPart::EtherType IP4Datagram::getEtherType() {
    return EtherType::IP4;
}

uint8_t IP4Datagram::copyTo(Kernel::NetworkByteBlock *output) {
    if (
            ip4DataPart == nullptr ||
            output == nullptr ||
            ip4DataPart->getLengthInBytes() > (size_t) (IP4DATAPART_MAX_LENGTH - header->getSize()) ||
            header->getSize() > IP4HEADER_MAX_LENGTH
            ) {
        return 1;
    }

    uint8_t errors = header->copyTo(output);

    //True if errors>0
    if (errors) {
        return errors;
    }

    //Call next level if no errors occurred yet
    return ip4DataPart->copyTo(output);
}