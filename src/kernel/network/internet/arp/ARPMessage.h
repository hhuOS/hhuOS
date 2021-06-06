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

    typedef struct arpMessage {
        arpheader_t header;
        uint8_t senderHardwareAddress[MAC_SIZE]{0, 0, 0, 0, 0, 0};
        uint8_t senderProtocolAddress[IP4ADDRESS_LENGTH]{0, 0, 0, 0};

        uint8_t targetHardwareAddress[MAC_SIZE]{0, 0, 0, 0, 0, 0};
        uint8_t targetProtocolAddress[IP4ADDRESS_LENGTH]{0, 0, 0, 0};
    } arpmessage_t;

    arpmessage_t message;

public:
    enum class OpCode {
        REQUEST = 1,
        REPLY = 2,
        INVALID
    };

    //Sending constructor
    explicit ARPMessage(OpCode opCode);

    //Incoming constructor
    ARPMessage() = default;

    ~ARPMessage() = default;

    static uint16_t getOpCodeAsInt(ARPMessage::OpCode opCode);


    OpCode getOpCode();

    static ARPMessage::OpCode parseOpCodeFromInteger(uint16_t value);

    void setSenderHardwareAddress(EthernetAddress *senderHardwareAddress);

    void setSenderProtocolAddress(IP4Address *senderProtocolAddress);

    void setTargetHardwareAddress(EthernetAddress *targetHardwareAddress);

    void setTargetProtocolAddress(IP4Address *targetProtocolAddress);

    uint8_t copyTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;

    EtherType getEtherType() override;

    uint8_t parse(NetworkByteBlock *input) override;

    void freeMemory() override;
};


#endif //HHUOS_ARPMESSAGE_H
