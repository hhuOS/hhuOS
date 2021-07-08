//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_ICMP4ECHO_H
#define HHUOS_ICMP4ECHO_H

#include <kernel/network/internet/icmp/ICMP4Message.h>
#include "ICMP4EchoReply.h"

class ICMP4Echo final : public ICMP4Message {
private:
    typedef struct icmp4echo {
        uint16_t identifier = 0;
        uint16_t sequenceNumber = 0;
    } echo_t;

    echo_t echoMessage;

    uint8_t do_copyTo(Kernel::NetworkByteBlock *output) final;

    uint8_t do_parse(Kernel::NetworkByteBlock *input) final;

    ICMP4MessageType do_getICMP4MessageType() final;

    size_t do_getLengthInBytes() final;

public:

    //Sending constructor
    ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber);

    ICMP4Echo() = default;

    ~ICMP4Echo() override = default;

    [[nodiscard]] ICMP4EchoReply *buildEchoReply() const;

    [[nodiscard]] uint16_t getSequenceNumber() const;
};


#endif //HHUOS_ICMP4ECHO_H
