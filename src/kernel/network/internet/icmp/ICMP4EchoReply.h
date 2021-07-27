//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4ECHOREPLY_H
#define HHUOS_ICMP4ECHOREPLY_H

#include <kernel/network/internet/icmp/ICMP4Message.h>

class ICMP4EchoReply final : public ICMP4Message {
private:
    typedef struct icmp4echoReply {
        uint16_t identifier = 0;
        uint16_t sequenceNumber = 0;
    } echoReplyMessage;

    echoReplyMessage echoReply;

    uint8_t do_copyTo(Kernel::NetworkByteBlock *output) final;

    uint8_t do_parse(Kernel::NetworkByteBlock *input) final;

    ICMP4MessageType do_getICMP4MessageType() final;

    uint16_t do_length() final;

public:

    //Sending constructor
    ICMP4EchoReply(uint16_t identifier, uint16_t sequenceNumber);

    ICMP4EchoReply() = default;

    ~ICMP4EchoReply() override = default;

    [[nodiscard]] uint16_t getIdentifier() const;

    [[nodiscard]] uint16_t getSequenceNumber() const;
};


#endif //HHUOS_ICMP4ECHOREPLY_H
