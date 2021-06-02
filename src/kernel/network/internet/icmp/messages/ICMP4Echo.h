//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_ICMP4ECHO_H
#define HHUOS_ICMP4ECHO_H

#include <kernel/network/internet/icmp/ICMP4Message.h>

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

public:
    //Sending constructor
    ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber);

    //Receiving constructor
    ICMP4Echo(NetworkByteBlock *input);

    uint8_t copyDataTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;

    uint16_t getIdentifier() const;

    uint16_t getSequenceNumber() const;

    ICMP4MessageType getICMP4MessageType() override;

    uint8_t parseInput();

    virtual ~ICMP4Echo();
};


#endif //HHUOS_ICMP4ECHO_H
