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

#define ARP_RESOLVE_SUCCESS 0
#define ARP_PROTOCOL_ADDRESS_NULL 11
#define ARP_TABLE_NULL 12

class ARPModule {
private:
    //HashMap did not work here, possible bug in HashMap implementation?
    Util::ArrayList<ARPEntry *> *arpTable;
    EthernetAddress *broadcastAddress;

public:
    ARPModule();

    virtual ~ARPModule();

    [[nodiscard]] EthernetAddress *getBroadcastAddress() const;

    void addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress);

    uint8_t resolveTo(EthernetAddress **ethernetAddress, IP4Address *ip4Address);
};


#endif //HHUOS_ARPMODULE_H
