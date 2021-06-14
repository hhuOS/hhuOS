//
// Created by hannes on 14.06.21.
//

#ifndef HHUOS_UDP4PORT_H
#define HHUOS_UDP4PORT_H


#include <cstdint>

class UDP4Port {
private:
    uint16_t number;

public:
    explicit UDP4Port(uint16_t number);

    void copyTo(uint16_t target) const;
};


#endif //HHUOS_UDP4PORT_H
