//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ARPMODULE_H
#define HHUOS_ARPMODULE_H

#include <lib/util/HashMap.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/ethernet/EthernetAddress.h>
#include "ARPEntry.h"

class ARPModule {
private:
    Util::ArrayList<ARPEntry *> *arpTable;

public:
    ARPModule();

    EthernetAddress *resolveIP4(IP4Address *ip4Address);

    void addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress);
};


#endif //HHUOS_ARPMODULE_H
