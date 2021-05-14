//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAPART_H
#define HHUOS_IP4DATAPART_H

#include <cinttypes>

class IP4DataPart {
private:
    void* data;
    uint16_t length;

public:
    IP4DataPart(void* data, uint16_t length);
};


#endif //HHUOS_IP4DATAPART_H
