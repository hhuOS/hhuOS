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

    uint8_t do_copyTo(Kernel::NetworkByteBlock *output) override;

public:

    //Sending constructor
    ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber);

    ICMP4Echo() = default;

    ~ICMP4Echo() override = default;

    ICMP4MessageType getICMP4MessageType() override;

    [[nodiscard]] ICMP4EchoReply *buildEchoReply() const;

    size_t getLengthInBytes() override;

    uint8_t parse(Kernel::NetworkByteBlock *input);

    [[nodiscard]] uint16_t getSequenceNumber() const;
};


#endif //HHUOS_ICMP4ECHO_H
