//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4ECHOREPLY_H
#define HHUOS_ICMP4ECHOREPLY_H


#include <kernel/network/internet/IP4DataPart.h>

class ICMP4EchoReply {
public:
    ICMP4EchoReply(IP4DataPart *dataPart);
};


#endif //HHUOS_ICMP4ECHOREPLY_H
