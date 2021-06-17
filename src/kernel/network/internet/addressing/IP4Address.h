//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4ADDRESS_H
#define HHUOS_IP4ADDRESS_H

#include "lib/libc/stdlib.h"
#include <lib/string/String.h>


class IP4Address {
private:
    uint8_t *address;

public:
    IP4Address(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);

    explicit IP4Address(const uint8_t *bytes);

    String asString();

    char *asChars();

    bool equals(IP4Address *other);

    void copyTo(uint8_t *target);

    IP4Address *calculateAND(const uint8_t netmask[4]);

    virtual ~IP4Address();
};


#endif //HHUOS_IP4ADDRESS_H
