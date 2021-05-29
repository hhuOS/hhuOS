//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_NETWORKDATAPART_H
#define HHUOS_NETWORKDATAPART_H

#include <cinttypes>
#include "IP4HeaderChecksum.h"

class IP4DataPart {
public:
    enum class IP4ProtocolType {
        ICMP4 = 1,
        UDP = 17
    };

    uint8_t getIP4ProtocolTypeAsInt(){
        return (uint8_t) getIP4ProtocolType();
    }

    virtual void *getMemoryAddress() = 0;

    virtual uint16_t getLengthInBytes() = 0;

    virtual IP4ProtocolType getIP4ProtocolType() = 0;
};


#endif //HHUOS_NETWORKDATAPART_H
