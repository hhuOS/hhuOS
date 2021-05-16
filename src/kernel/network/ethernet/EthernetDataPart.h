//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ETHERNETDATAPART_H
#define HHUOS_ETHERNETDATAPART_H

#include "EtherType.h"

class EthernetDataPart {
public:
    virtual void *getMemoryAddress() = 0;

    virtual uint16_t getLength() = 0;

    virtual EtherType getEtherType() = 0;
};


#endif //HHUOS_ETHERNETDATAPART_H
