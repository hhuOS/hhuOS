//
// Created by hannes on 17.05.21.
//

#include "UDP4Datagram.h"

UDP4Datagram::UDP4Datagram(uint16_t destinationPort, UDP4DataPart *udp4DataPart) {
    header.destinationPort=destinationPort;
    this->udp4DataPart = udp4DataPart;
}

uint8_t UDP4Datagram::copyTo(NetworkByteBlock *output) {
    if (
            udp4DataPart == nullptr ||
            output == nullptr ||
            udp4DataPart->getLengthInBytes() > (size_t) (UDP4DATAPART_MAX_LENGTH - sizeof header) ||
            sizeof header > UDP4HEADER_MAX_LENGTH
            ) {
        return 1;
    }

    uint8_t errors = 0;
    errors += output->append(header.sourcePort);
    errors += output->append(header.destinationPort);
    errors += output->append(header.length);
    errors += output->append(header.checksum);

    //True if errors>0
    if (errors) {
        return errors;
    }

    //Call next level if no errors occurred yet
    return udp4DataPart->copyTo(output);
}

size_t UDP4Datagram::getLengthInBytes() {
    return sizeof header + udp4DataPart->getLengthInBytes();
}

IP4DataPart::IP4ProtocolType UDP4Datagram::getIP4ProtocolType() {
    return IP4ProtocolType::UDP;
}

uint8_t UDP4Datagram::parseHeader(NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < sizeof this->header) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->read(&header.sourcePort);
    errors += input->read(&header.destinationPort);
    errors += input->read(&header.length);
    errors += input->read(&header.checksum);

    return errors;
}
