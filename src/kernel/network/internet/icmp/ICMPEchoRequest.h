//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_ICMPECHOREQUEST_H
#define HHUOS_ICMPECHOREQUEST_H

#include <cinttypes>
#include <kernel/network/NetworkDataPart.h>

class ICMPEchoRequest : public NetworkDataPart{
public:
    ICMPEchoRequest();

    void *getData() override;
    uint16_t getLength() override;
};


#endif //HHUOS_ICMPECHOREQUEST_H
