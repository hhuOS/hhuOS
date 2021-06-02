//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_ICMP4ECHO_H
#define HHUOS_ICMP4ECHO_H

#include <kernel/network/internet/icmp/ICMP4Message.h>
#include "ICMP4EchoReply.h"

class ICMP4Echo : public ICMP4Message {
private:
    typedef struct icmp4echo {
        uint8_t type = 8; //8 for echo, 0 for echo reply (RFC792)
        uint8_t code = 0;
        uint16_t checksum = 0;
        uint16_t identifier = 0;
        uint16_t sequenceNumber = 0;
    } echo_t;

    echo_t echoMessage;

    NetworkByteBlock *input;
    IP4Address *destinationAddress = nullptr;
    IP4Address *sourceAddress = nullptr;
public:
    //Sending constructor
    ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber);

    //Receiving constructor
    ICMP4Echo(IP4Address *destinationAddress, IP4Address *sourceAddress, NetworkByteBlock *input);

    uint8_t copyDataTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;

    IP4Address *getSourceAddress() const;

    ICMP4MessageType getICMP4MessageType() override;

    uint8_t parseInput() override;

    virtual ~ICMP4Echo();

    ICMP4EchoReply *buildEchoReply();
};


#endif //HHUOS_ICMP4ECHO_H
