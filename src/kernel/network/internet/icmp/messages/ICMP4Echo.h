//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_ICMP4ECHO_H
#define HHUOS_ICMP4ECHO_H

#include <cinttypes>
#include <kernel/network/internet/IP4DataPart.h>

class ICMP4Echo : public IP4DataPart {
public:
    ICMP4Echo();

    void *getMemoryAddress() override;

    uint16_t getLength() override;

    IP4ProtocolType getIP4ProtocolType() override;
};


#endif //HHUOS_ICMP4ECHO_H
