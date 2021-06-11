//
// Created by hannes on 10.06.21.
//

#include "UDP4Port.h"

UDP4Port::UDP4Port(uint16_t portNumber) : portNumber(portNumber) {}

bool UDP4Port::equals(UDP4Port *other) {
    return other->portNumber == this->portNumber;
}
