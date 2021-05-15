//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_NETWORKDATAPART_H
#define HHUOS_NETWORKDATAPART_H

#include <cinttypes>

class NetworkDataPart {
public:
    virtual void *getData() = 0;

    virtual uint16_t getLength() = 0;
};


#endif //HHUOS_NETWORKDATAPART_H
