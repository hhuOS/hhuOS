//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ETHERNETDATAPART_H
#define HHUOS_ETHERNETDATAPART_H

#include <kernel/network/NetworkByteBlock.h>

class EthernetDataPart {
public:
    //Relevant EtherTypes -> list available in RFC7042 Appendix B (pages 25,26)
    enum EtherType : uint16_t {
        IP4 = 0x0800,
        ARP = 0x0806,
        IP6 = 0x86dd,
        INVALID = 0
    };

    static EtherType parseIntAsEtherType(uint16_t value) {
        switch (value) {
            case 0x0800:
                return EthernetDataPart::EtherType::IP4;
            case 0x0806:
                return EthernetDataPart::EtherType::ARP;
            case 0x86dd:
                return EthernetDataPart::EtherType::IP6;
            default:
                return EthernetDataPart::EtherType::INVALID;
        }
    }

    virtual uint8_t copyTo(Kernel::NetworkByteBlock *output) = 0;

    virtual size_t getLengthInBytes() = 0;

    virtual EtherType getEtherType() = 0;

    virtual String asString(String spacing) = 0;

    static String etherTypeAsString(EtherType type) {
        switch (type) {
            case EtherType::IP4:
                return "'IPv4'";
            case EtherType::ARP:
                return "'ARP'";
            case EtherType::IP6:
                return "'IPv6'";
            default:
                return "'INVALID'";
        }
    }
};


#endif //HHUOS_ETHERNETDATAPART_H
