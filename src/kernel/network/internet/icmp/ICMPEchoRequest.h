//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_ICMPECHOREQUEST_H
#define HHUOS_ICMPECHOREQUEST_H

#include <cinttypes>
#include <kernel/network/internet/IP4DataPart.h>

class ICMPEchoRequest : public IP4DataPart {
public:
    ICMPEchoRequest();

    void *getMemoryAddress() override;

    uint16_t getLength() override;

    IP4ProtocolType getIP4ProtocolType() override;
};


#endif //HHUOS_ICMPECHOREQUEST_H
