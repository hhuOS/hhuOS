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

    IP4ProtocolType getIP4ProtocolType() override {
        return IP4ProtocolType::ICMP4;
    }

    uint8_t copyTo(Kernel::NetworkByteBlock *byteBlock) override = 0;

    size_t getLengthInBytes() override = 0;

    virtual ICMP4MessageType getICMP4MessageType() = 0;

    uint8_t fillChecksumField() {
        if (header.checksum != 0) {
            //Header checksum already set!
            return 1;
        }

        auto *headerAsBytes = new Kernel::NetworkByteBlock(getLengthInBytes());
        if (this->copyTo(headerAsBytes)) {
            delete headerAsBytes;
            return 1;
        }
        headerAsBytes->resetIndex();

        uint16_t calculationResult = 0;
        if (IP4Header::calculateInternetChecksum(&calculationResult, headerAsBytes)) {
            delete headerAsBytes;
            return 1;
        }
        header.checksum = calculationResult;
        delete headerAsBytes;
        return 0;
    }

    bool checksumIsValid() {
        if (header.checksum == 0) {
            //Header checksum not parsed!
            return false;
        }

        uint16_t calculationResult = 0;
        auto *headerAsBytes = new Kernel::NetworkByteBlock(getLengthInBytes());
        if (this->copyTo(headerAsBytes)) {
            delete headerAsBytes;
            return false;
        }
        headerAsBytes->resetIndex();

        if (IP4Header::calculateInternetChecksum(&calculationResult, headerAsBytes)) {
            delete headerAsBytes;
            return false;
        }

        delete headerAsBytes;
        return calculationResult == 0;
    }
};

#endif //HHUOS_ICMP4MESSAGE_H
