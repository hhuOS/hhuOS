//
// Created by hannes on 10.06.21.
//

#ifndef HHUOS_UDP4PORT_H
#define HHUOS_UDP4PORT_H


#include <cstdint>

class UDP4Port {
private:
    uint16_t portNumber;

public:
    explicit UDP4Port(uint16_t portNumber);

    bool equals(UDP4Port *other);
};


#endif //HHUOS_UDP4PORT_H
