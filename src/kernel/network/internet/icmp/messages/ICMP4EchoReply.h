//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4ECHOREPLY_H
#define HHUOS_ICMP4ECHOREPLY_H

#include <kernel/network/internet/icmp/ICMP4Message.h>
#include <kernel/network/NetworkByteBlock.h>

class ICMP4EchoReply final : public ICMP4Message {
private:
    typedef struct icmp4echoReply {
        uint8_t type = 0; //0 for echo reply, 8 for echo (RFC792)
        uint8_t code = 0;
        uint16_t checksum = 0;
        uint16_t identifier = 0;
        uint16_t sequenceNumber = 0;
    } echoReplyMessage;

    typedef struct ip4information {
        uint8_t sourceAddress[IP4ADDRESS_LENGTH]{0, 0, 0, 0};
    } ip4info_t;

    echoReplyMessage echoReply;
    ip4info_t ip4Info;

public:
    //Sending constructor
    ICMP4EchoReply(uint16_t identifier, uint16_t sequenceNumber);

    ICMP4EchoReply() = default;

    ~ICMP4EchoReply() = default;

    uint8_t copyTo(NetworkByteBlock *output) override;

    size_t getLengthInBytes() override;

    [[nodiscard]] uint16_t getIdentifier() const;

    [[nodiscard]] uint16_t getSequenceNumber() const;

    [[nodiscard]] IP4Address *getSourceAddress() const;

    void setSourceAddress(IP4Address *sourceAddress);

    ICMP4MessageType getICMP4MessageType() override;

    uint8_t parseHeader(NetworkByteBlock *input) override;

    IP4ProtocolType getIP4ProtocolType() override;
};


#endif //HHUOS_ICMP4ECHOREPLY_H
