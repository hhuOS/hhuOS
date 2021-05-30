//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ETHERNETDATAPART_H
#define HHUOS_ETHERNETDATAPART_H

class EthernetDataPart {
public:
    //Relevant EtherTypes -> full list available in RFC7042 Appendix B
    enum class EtherType {
        IP4 = 0x0800,
        ARP = 0x0806,
        IP6 = 0x86dd,
        INVALID = 0
    };

    uint16_t getEtherTypeAsInt(){
        return (uint16_t) getEtherType();
    }

    static EtherType parseIntAsEtherType(uint16_t value) {
        switch (value) {
            case 0x0800: return EthernetDataPart::EtherType::IP4;
            case 0x0806: return EthernetDataPart::EtherType::ARP;
            case 0x86dd: return EthernetDataPart::EtherType::IP6;
            default: return EthernetDataPart::EtherType::INVALID;
        }
    }

    virtual uint8_t copyDataTo(NetworkByteBlock *byteBlock) = 0;

    virtual uint16_t getLengthInBytes() = 0;

    virtual EtherType getEtherType() = 0;
};


#endif //HHUOS_ETHERNETDATAPART_H
