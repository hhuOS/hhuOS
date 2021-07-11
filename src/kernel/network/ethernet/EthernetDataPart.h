//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ETHERNETDATAPART_H
#define HHUOS_ETHERNETDATAPART_H

#include <kernel/network/NetworkByteBlock.h>

class EthernetDataPart {
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

    virtual ~EthernetDataPart();

    // forbid copying
    EthernetDataPart(EthernetDataPart const &) = delete;

    EthernetDataPart &operator=(EthernetDataPart const &) = delete;

    static EtherType parseIntAsEtherType(uint16_t value);

    EtherType getEtherType();

    uint16_t length();

    uint8_t copyTo(Kernel::NetworkByteBlock *output);

private:
    virtual uint8_t do_copyTo(Kernel::NetworkByteBlock *output) = 0;

    virtual EtherType do_getEtherType() = 0;

    virtual uint16_t do_length() = 0;
};


#endif //HHUOS_ETHERNETDATAPART_H
