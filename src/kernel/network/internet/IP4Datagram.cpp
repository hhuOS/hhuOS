//
// Created by hannes on 14.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/network/internet/icmp/messages/ICMP4DestinationUnreachable.h>
#include <kernel/network/internet/icmp/messages/ICMP4TimeExceeded.h>
#include <kernel/network/internet/icmp/messages/ICMP4EchoReply.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    destinationAddress->copyTo(header.destinationAddress);
    header.protocolType = ip4DataPart->getIP4ProtocolTypeAsInt();
    this->ip4DataPart = ip4DataPart;

    header.totalLength = sizeof(header) + ip4DataPart->getLengthInBytes();
}

IP4Datagram::~IP4Datagram() {
    delete ip4DataPart;
}

IP4DataPart::IP4ProtocolType IP4Datagram::getIP4ProtocolType() const {
    return ip4DataPart->getIP4ProtocolType();
}

IP4Address *IP4Datagram::getSourceAddress() const {
    return new IP4Address(header.sourceAddress);
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    return new IP4Address(header.destinationAddress);
}

void IP4Datagram::setSourceAddress(IP4Address *source) {
    source->copyTo(header.sourceAddress);
}

size_t IP4Datagram::getLengthInBytes() {
    return header.totalLength;
}

EthernetDataPart::EtherType IP4Datagram::getEtherType() {
    return EtherType::IP4;
}

IP4DataPart *IP4Datagram::getIP4DataPart() const {
    return ip4DataPart;
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

uint8_t IP4Datagram::parse(NetworkByteBlock *input) {
    if (input == nullptr ||
        input->bytesRemaining() <= sizeof header
            ) {
        return 1;
    }
    //TODO: Add check for correct size! It must fail if header is larger that 20 bytes
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

    switch (IP4DataPart::parseIntAsIP4ProtocolType(header.protocolType)) {
        case IP4DataPart::IP4ProtocolType::ICMP4: {
            uint8_t typeByte = 0;
            input->read(&typeByte);
            switch (ICMP4Message::parseByteAsICMP4MessageType(typeByte)) {
                case ICMP4Message::ICMP4MessageType::ECHO_REPLY:
                    this->ip4DataPart = new ICMP4EchoReply();
                    break;
                case ICMP4Message::ICMP4MessageType::DESTINATION_UNREACHABLE:
                    this->ip4DataPart = new ICMP4DestinationUnreachable();
                    break;
                case ICMP4Message::ICMP4MessageType::ECHO:
                    this->ip4DataPart = new ICMP4Echo();
                    break;
                case ICMP4Message::ICMP4MessageType::TIME_EXCEEDED:
                    this->ip4DataPart = new ICMP4TimeExceeded();
                    break;
                default:
                    errors++;
            }
            break;
        }
        case IP4DataPart::IP4ProtocolType::UDP: {
            this->ip4DataPart = new UDPDatagram();
            break;
        }
        default:
            errors++;
    }

    //True if errors>0
    if (errors) {
        return errors;
    }

    //Call next level if no errors occurred yet
    return ip4DataPart->parse(input);
}
