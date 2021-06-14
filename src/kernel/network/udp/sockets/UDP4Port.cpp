//
// Created by hannes on 14.06.21.
//

#include "UDP4Port.h"

UDP4Port::UDP4Port(uint16_t number) {
    this->number=number;
}

void UDP4Port::copyTo(uint16_t target) const {
    target=this->number;
}
