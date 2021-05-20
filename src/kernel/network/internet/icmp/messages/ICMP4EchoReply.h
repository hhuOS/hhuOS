//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4ECHOREPLY_H
#define HHUOS_ICMP4ECHOREPLY_H

#include <kernel/network/internet/icmp/ICMP4Message.h>

class ICMP4EchoReply : public ICMP4Message {
private:
    typedef struct icmp4echoReply {
        uint8_t type;
        uint8_t code;
        uint16_t checksum;
        uint16_t identifier;
        uint16_t sequenceNumber;
    } echoReplyMessage;

    echoReplyMessage myMessage;
public:
    //Sending constructor
    ICMP4EchoReply(uint16_t identifier, uint16_t sequenceNumber);

    //Receiving constructor
    ICMP4EchoReply(IP4DataPart *dataPart);

    void *getMemoryAddress() override;

    uint16_t getLengthInBytes() override;

    uint16_t getIdentifier();

    uint16_t getSequenceNumber();
};


#endif //HHUOS_ICMP4ECHOREPLY_H
