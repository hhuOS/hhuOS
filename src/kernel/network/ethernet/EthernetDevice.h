//
// Created by hannes on 25.05.21.
//

#ifndef HHUOS_ETHERNETDEVICE_H
#define HHUOS_ETHERNETDEVICE_H


#include "EthernetAddress.h"
#include "EthernetFrame.h"

class EthernetDevice {
public:
    virtual EthernetAddress *getEthernetAddress() = 0;

    virtual void sendEthernetFrame(EthernetFrame *ethernetFrame) = 0;
};


#endif //HHUOS_ETHERNETDEVICE_H
