#include "Address.h"

Address::Address() : address(0) {

}

Address::Address(uint32_t address) : address(address) {

}

Address::operator uint32_t() const {

    return address;
}

bool Address::operator!=(const Address &other) const {

    return address != other.address;
}

bool Address::operator==(const Address &other) const {

    return address == other.address;
}


