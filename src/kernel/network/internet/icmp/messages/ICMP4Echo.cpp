//
// Created by hannes on 14.05.21.
//

#include "ICMP4Echo.h"

ICMP4Echo::ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber) {
    myMessage.type = 8; //8 for echo, 0 for echo reply (RFC792)
    myMessage.code = 0;
    myMessage.checksum = 0;
    myMessage.identifier = identifier;
    myMessage.sequenceNumber = sequenceNumber;
}

ICMP4Echo::ICMP4Echo(IP4DataPart *dataPart) {
}

uint8_t ICMP4Echo::copyDataTo(uint8_t *byteBlock) {
    //TODO: Implement this one!
    return 1;
}

uint16_t ICMP4Echo::getLengthInBytes() {
    return sizeof(echo_t);
}

uint16_t ICMP4Echo::getIdentifier() {
    return myMessage.identifier;
}

uint16_t ICMP4Echo::getSequenceNumber() {
    return myMessage.sequenceNumber;
}

ICMP4Message::ICMP4MessageType ICMP4Echo::getICMP4MessageType() {
    return ICMP4MessageType::ECHO;
}
