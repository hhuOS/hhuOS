//
// Created by hannes on 18.05.21.
//

#ifndef HHUOS_ICMP4MESSAGE_H
#define HHUOS_ICMP4MESSAGE_H

#include <cinttypes>
#include <kernel/network/internet/IP4DataPart.h>
#include <kernel/network/internet/icmp/ICMP4MessageType.h>

class ICMP4Message : public IP4DataPart {

public:
    IP4ProtocolType getIP4ProtocolType() override {
        return IP4ProtocolType::ICMP4;
    }

    static ICMP4MessageType parseMessageType(uint8_t type) {
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
                return ICMP4MessageType::INVALID_MESSAGE_TYPE;
        }
    }
};


#endif //HHUOS_ICMP4MESSAGE_H
