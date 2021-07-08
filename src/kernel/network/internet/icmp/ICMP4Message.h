//
// Created by hannes on 18.05.21.
//

#ifndef HHUOS_ICMP4MESSAGE_H
#define HHUOS_ICMP4MESSAGE_H

#include <kernel/network/internet/IP4DataPart.h>
#include <kernel/network/internet/IP4Header.h>

class ICMP4Message : public IP4DataPart {
protected:
    typedef struct basicICMP4Header {
        uint8_t type = 0;
        uint8_t code = 0;
        uint16_t checksum = 0;
    } header_t;

    header_t header;

    ICMP4Message();

public:
    //see RFC 792 for details
    enum class ICMP4MessageType {
        ECHO_REPLY = 0,
        DESTINATION_UNREACHABLE = 3,
        SOURCE_QUENCH = 4,
        REDIRECT = 5,
        ECHO = 8,
        TIME_EXCEEDED = 11,
        PARAMETER_PROBLEM = 12,
        TIMESTAMP = 13,
        TIMESTAMP_REPLY = 14,
        INFORMATION_REQUEST = 15,
        INFORMATION_REPLY = 16,
        INVALID
    };

    ~ICMP4Message() override;

    // forbid copying
    ICMP4Message(ICMP4Message const &) = delete;

    ICMP4Message &operator=(ICMP4Message const &) = delete;

    static ICMP4MessageType parseByteAsICMP4MessageType(uint8_t type);

    bool checksumIsValid();

    uint8_t fillChecksumField();

    uint8_t parse(Kernel::NetworkByteBlock *input);

    ICMP4MessageType getICMP4MessageType();

private:
    virtual uint8_t do_parse(Kernel::NetworkByteBlock *input) = 0;

    virtual ICMP4MessageType do_getICMP4MessageType() = 0;

    IP4ProtocolType do_getIP4ProtocolType() final;
};

#endif //HHUOS_ICMP4MESSAGE_H
