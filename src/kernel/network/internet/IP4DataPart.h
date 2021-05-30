//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_NETWORKDATAPART_H
#define HHUOS_NETWORKDATAPART_H

#include "lib/libc/stdlib.h"
#include "IP4HeaderChecksum.h"

class IP4DataPart {
public:
    enum class IP4ProtocolType {
        ICMP4 = 1,
        UDP = 17,
        INVALID = 0
    };

    uint8_t getIP4ProtocolTypeAsInt(){
        return (uint8_t) getIP4ProtocolType();
    }

    static IP4ProtocolType parseIntAsIP4ProtocolType(uint8_t value){
        switch (value) {
            case 1: return IP4ProtocolType::ICMP4;
            case 17: return IP4ProtocolType::UDP;
            default: return IP4ProtocolType::INVALID;
        }
    }

    virtual void * getMemoryAddress() = 0;

    virtual uint8_t copyDataTo(uint8_t *byteBlock) = 0;

    virtual uint16_t getLengthInBytes() = 0;

    virtual IP4ProtocolType getIP4ProtocolType() = 0;
};


#endif //HHUOS_NETWORKDATAPART_H
