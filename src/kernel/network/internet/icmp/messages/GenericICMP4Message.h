//
// Created by hannes on 02.06.21.
//

#ifndef HHUOS_GENERICICMP4MESSAGE_H
#define HHUOS_GENERICICMP4MESSAGE_H


#include <kernel/network/internet/icmp/ICMP4Message.h>
#include <kernel/network/internet/addressing/IP4Address.h>

class GenericICMP4Message : public ICMP4Message{
private:
    IP4Address *destinationAddress = nullptr;
    IP4Address *sourceAddress = nullptr;
    NetworkByteBlock *input = nullptr;
public:
    GenericICMP4Message(IP4Address *destinationAddress, IP4Address *sourceAddress, NetworkByteBlock *input);

    uint8_t parseInput();

    ICMP4MessageType getICMP4MessageType() override;

    virtual ~GenericICMP4Message();

    IP4ProtocolType getIP4ProtocolType() override;

    uint8_t copyDataTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;
};


#endif //HHUOS_GENERICICMP4MESSAGE_H
