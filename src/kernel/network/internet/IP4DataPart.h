//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAPART_H
#define HHUOS_IP4DATAPART_H

#include <cinttypes>

class IP4DataPart {
public:
    virtual void *getData() = 0;
    virtual uint16_t getLength() = 0;
};


#endif //HHUOS_IP4DATAPART_H
