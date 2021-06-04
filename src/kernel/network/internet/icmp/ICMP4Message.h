//
// Created by hannes on 18.05.21.
//

#ifndef HHUOS_ICMP4MESSAGE_H
#define HHUOS_ICMP4MESSAGE_H

#include "lib/libc/stdlib.h"
#include <kernel/network/internet/IP4DataPart.h>

class ICMP4Message : public IP4DataPart {
public:
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

    IP4ProtocolType getIP4ProtocolType() override {
        return IP4ProtocolType::ICMP4;
    }

    uint8_t getICMP4MessageTypeAsInt() {
        return (uint8_t) getICMP4MessageType();
    }

    uint8_t copyTo(NetworkByteBlock *byteBlock) override = 0;

    size_t getLengthInBytes() override = 0;

    virtual ICMP4MessageType getICMP4MessageType() = 0;

    uint8_t parse(NetworkByteBlock *input) override {
        return 0;
    }

    void freeMemory() override {

    }

    static ICMP4MessageType parseByteAsICMP4MessageType(uint8_t type) {
        switch (type) {
            case 0:
                return ICMP4MessageType::ECHO_REPLY;
            case 3:
                return ICMP4MessageType::DESTINATION_UNREACHABLE;
            case 4:
                return ICMP4MessageType::SOURCE_QUENCH;
            case 5:
                return ICMP4MessageType::REDIRECT;
            case 8:
                return ICMP4MessageType::ECHO;
            case 11:
                return ICMP4MessageType::TIME_EXCEEDED;
            case 12:
                return ICMP4MessageType::PARAMETER_PROBLEM;
            case 13:
                return ICMP4MessageType::TIMESTAMP;
            case 14:
                return ICMP4MessageType::TIMESTAMP_REPLY;
            case 15:
                return ICMP4MessageType::INFORMATION_REQUEST;
            case 16:
                return ICMP4MessageType::INFORMATION_REPLY;
            default:
                return ICMP4MessageType::INVALID;
        }
    }
};


#endif //HHUOS_ICMP4MESSAGE_H
