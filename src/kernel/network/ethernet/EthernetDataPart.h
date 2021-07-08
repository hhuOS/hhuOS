//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ETHERNETDATAPART_H
#define HHUOS_ETHERNETDATAPART_H

#include <kernel/network/NetworkTransmittable.h>

class EthernetDataPart : public NetworkTransmittable {
protected:
    EthernetDataPart();

public:
    //Relevant EtherTypes -> list available in RFC7042 Appendix B (pages 25,26)
    enum EtherType : uint16_t {
        IP4 = 0x0800,
        ARP = 0x0806,
        IP6 = 0x86dd,
        INVALID = 0
    };

    ~EthernetDataPart() override;

    // forbid copying
    EthernetDataPart(EthernetDataPart const &) = delete;

    EthernetDataPart &operator=(EthernetDataPart const &) = delete;

    static EtherType parseIntAsEtherType(uint16_t value);

    EtherType getEtherType();

private:
    virtual EtherType do_getEtherType() = 0;
};


#endif //HHUOS_ETHERNETDATAPART_H
