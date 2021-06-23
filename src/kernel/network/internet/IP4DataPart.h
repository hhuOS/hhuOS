//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAPART_H
#define HHUOS_IP4DATAPART_H

#include <kernel/network/NetworkByteBlock.h>

class IP4DataPart {
public:
    enum class IP4ProtocolType {
        ICMP4 = 1,
        UDP = 17,
        INVALID = 0
    };

    static IP4ProtocolType parseIntAsIP4ProtocolType(uint8_t value) {
        switch (value) {
            case 1:
                return IP4ProtocolType::ICMP4;
            case 17:
                return IP4ProtocolType::UDP;
            default:
                return IP4ProtocolType::INVALID;
        }
    }

    virtual uint8_t copyTo(Kernel::NetworkByteBlock *byteBlock) = 0;

    virtual size_t getLengthInBytes() = 0;

    virtual IP4ProtocolType getIP4ProtocolType() = 0;
};


#endif //HHUOS_IP4DATAPART_H
