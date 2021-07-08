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
        uint16_t identifier = 0;
        uint16_t sequenceNumber = 0;
    } echoReplyMessage;

    echoReplyMessage echoReply;

    uint8_t do_copyTo(Kernel::NetworkByteBlock *output) override;

public:

    //Sending constructor
    ICMP4EchoReply(uint16_t identifier, uint16_t sequenceNumber);

    ICMP4EchoReply() = default;

    ~ICMP4EchoReply() override = default;

    ICMP4MessageType getICMP4MessageType() override;

    size_t getLengthInBytes() override;

    uint8_t parse(Kernel::NetworkByteBlock *input);

    [[nodiscard]] uint16_t getIdentifier() const;

    [[nodiscard]] uint16_t getSequenceNumber() const;
};


#endif //HHUOS_ICMP4ECHOREPLY_H
