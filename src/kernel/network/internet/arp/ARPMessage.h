//
// Created by hannes on 02.06.21.
//

#ifndef HHUOS_ARPMESSAGE_H
#define HHUOS_ARPMESSAGE_H


#include <kernel/network/NetworkByteBlock.h>
#include <kernel/network/ethernet/EthernetDataPart.h>
#include <kernel/network/ethernet/EthernetAddress.h>
#include <kernel/network/internet/addressing/IP4Address.h>

class ARPMessage : public EthernetDataPart {
private:
    typedef struct arpHeader {
        uint16_t hardwareType = 1; //1 is for Ethernet
        uint16_t protocolType = 0x0800; //Value for IPv4
        uint8_t hardwareAddressLength = MAC_SIZE;
        uint8_t protocolAddressLength = IP4ADDRESS_LENGTH;
        uint16_t opCode = 0; //1 is REQUEST, 2 is REPLY, 0 is undefined here
    } arpheader_t;

    arpheader_t header;
    EthernetAddress *senderHardwareAddress = nullptr;
    IP4Address *senderProtocolAddress = nullptr;

    EthernetAddress *targetHardwareAddress = nullptr;
    IP4Address *targetProtocolAddress = nullptr;

public:
    //Request constructor
    ARPMessage(IP4Address *targetProtocolAddress, EthernetAddress *senderHardwareAddress, IP4Address *senderProtocolAddress);

    //Incoming constructor
    ARPMessage() = default;

    ~ARPMessage() override = default;

    uint8_t copyTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;

    EtherType getEtherType() override;

    uint8_t parse(NetworkByteBlock *input) override;
};


#endif //HHUOS_ARPMESSAGE_H
