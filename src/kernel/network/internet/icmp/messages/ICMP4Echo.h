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

    typedef struct ip4information {
        uint8_t sourceAddress[IP4ADDRESS_LENGTH]{0,0,0,0};
    } ip4info_t;

    echo_t echoMessage;
    ip4info_t ip4Info;

public:
    //Sending constructor
    ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber);

    void setSourceAddress(IP4Address *ip4Address);

    IP4Address *getSourceAddress();

    uint8_t copyTo(NetworkByteBlock *output) override;

    size_t getLengthInBytes() override;

    ICMP4MessageType getICMP4MessageType() override;

    uint8_t parse(NetworkByteBlock *input) override;

    [[nodiscard]] ICMP4EchoReply *buildEchoReply() const;
};


#endif //HHUOS_ICMP4ECHO_H
