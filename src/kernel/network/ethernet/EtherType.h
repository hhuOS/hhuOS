//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ETHERTYPE_H
#define HHUOS_ETHERTYPE_H

//Relevant Ethertypes -> full list available in RFC7042 Appendix B
enum class EtherType {
    IP4 = 0x0800,
    ARP = 0x0806,
    IP6 = 0x86dd,
    INVALID = 0
};


#endif //HHUOS_ETHERTYPE_H
