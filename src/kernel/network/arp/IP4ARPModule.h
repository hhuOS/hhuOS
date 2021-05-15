//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_IP4ARPMODULE_H
#define HHUOS_IP4ARPMODULE_H

#include <lib/util/HashMap.h>
#include <kernel/network/internet/IP4Address.h>
#include <kernel/network/ethernet/EthernetAddress.h>

class IP4ARPModule {
private:
    Util::HashMap<IP4Address *, EthernetAddress *> arpTable;

    void sendARPRequest(IP4Address *ip4Address);

public:
    EthernetAddress *resolveIP4(IP4Address *ip4Address);
};


#endif //HHUOS_IP4ARPMODULE_H
