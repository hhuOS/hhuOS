//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ARPMODULE_H
#define HHUOS_ARPMODULE_H

#include <lib/util/HashMap.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/ethernet/EthernetAddress.h>
#include <kernel/network/ethernet/EthernetFrame.h>
#include <kernel/network/internet/IP4Datagram.h>
#include "ARPEntry.h"

class ARPModule {
private:
    //HashMap did not work here, possible bug in HashMap implementation?
    Util::ArrayList<ARPEntry *> *arpTable;
    EthernetAddress *broadcastAddress;

public:
    ARPModule();

    [[nodiscard]] EthernetAddress *getBroadcastAddress() const;

    void addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress);

    EthernetAddress *resolveIP4(IP4Address *ip4Address);
};


#endif //HHUOS_ARPMODULE_H
