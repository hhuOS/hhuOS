//
// Created by hannes on 14.05.21.
//

#include <kernel/network/internet/icmp/messages/ICMP4EchoReply.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    destinationAddress->copyTo(header.destinationAddress);
    header.protocolType = ip4DataPart->getIP4ProtocolTypeAsInt();
    this->ip4DataPart = ip4DataPart;

    header.totalLength = sizeof header + ip4DataPart->getLengthInBytes();
}

IP4Datagram::~IP4Datagram() {
    delete sourceAddress;
    //dataPart is null if this datagram is an incoming one!
    //-> deleting is only necessary in an outgoing datagram
    if (ip4DataPart == nullptr) {
        return;
    }
    switch (IP4DataPart::parseIntAsIP4ProtocolType(header.protocolType)) {
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
            delete (UDPDatagram *) ip4DataPart;
            break;
        }
        default:
            break;
    }
}

IP4DataPart::IP4ProtocolType IP4Datagram::getIP4ProtocolType() const {
    //get IP4ProtocolType via header, dataPart can be null!
    return IP4DataPart::parseIntAsIP4ProtocolType(header.protocolType);
}

IP4Address *IP4Datagram::getSourceAddress() const {
    return this->sourceAddress;
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    return new IP4Address(header.destinationAddress);
}

void IP4Datagram::setSourceAddress(IP4Address *source) {
    source->copyTo(header.sourceAddress);
    this->sourceAddress = new IP4Address(header.sourceAddress);
}

size_t IP4Datagram::getLengthInBytes() {
    return header.totalLength;
}

EthernetDataPart::EtherType IP4Datagram::getEtherType() {
    return EtherType::IP4;
}

size_t IP4Datagram::getHeaderLengthInBytes() const {
    return (size_t) (header.version_headerLength - 0x40) * 4;
}

uint8_t IP4Datagram::copyTo(NetworkByteBlock *output) {
    if (
            ip4DataPart == nullptr ||
            output == nullptr ||
            ip4DataPart->getLengthInBytes() > (size_t) (IP4DATAPART_MAX_LENGTH - sizeof header) ||
            sizeof header > IP4HEADER_MAX_LENGTH
            ) {
        return 1;
    }

    uint8_t errors = 0;
    errors += output->append(header.version_headerLength);
    errors += output->append(header.typeOfService);
    errors += output->append(header.totalLength);
    errors += output->append(header.identification);
    errors += output->append(header.flags_fragmentOffset);
    errors += output->append(header.timeToLive);
    errors += output->append(header.protocolType);
    errors += output->append(header.headerChecksum);
    errors += output->append(header.sourceAddress, IP4ADDRESS_LENGTH);
    errors += output->append(header.destinationAddress, IP4ADDRESS_LENGTH);

    //True if errors>0
    if (errors) {
        return errors;
    }

    //Call next level if no errors occurred yet
    return ip4DataPart->copyTo(output);
}

uint8_t IP4Datagram::parseHeader(NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < sizeof this->header) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->read(&header.version_headerLength);
    errors += input->read(&header.typeOfService);
    errors += input->read(&header.totalLength);
    errors += input->read(&header.identification);
    errors += input->read(&header.flags_fragmentOffset);
    errors += input->read(&header.timeToLive);
    errors += input->read(&header.protocolType);
    errors += input->read(&header.headerChecksum);
    errors += input->read(header.sourceAddress, IP4ADDRESS_LENGTH);
    errors += input->read(header.destinationAddress, IP4ADDRESS_LENGTH);

    //Skip additional bytes if incoming header is larger than our internal one
    //-> next layer would read our remaining header bytes as data otherwise!
    uint8_t remainingHeaderBytes = getHeaderLengthInBytes() - sizeof this->header;
    //True if remainingHeaderBytes > 0
    if (remainingHeaderBytes) {
        errors += input->skip(remainingHeaderBytes);
    }

    if(sourceAddress!= nullptr){
        //already initialized!
        errors++;
        return errors;
    }

    this->sourceAddress=new IP4Address(header.sourceAddress);

    return errors;
}
