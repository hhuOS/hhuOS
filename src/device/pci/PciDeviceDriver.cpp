#include "PciDeviceDriver.h"

Util::Array<Util::Pair<uint16_t, uint16_t>> PciDeviceDriver::getIdPairs() const {
    Util::Pair<uint16_t, uint16_t> pair(0, 0);
    Util::Array<Util::Pair<uint16_t, uint16_t>> ret(1);

    ret[0] = pair;

    return ret;
};

uint8_t PciDeviceDriver::getBaseClass() const {
    return 0;
};

uint8_t PciDeviceDriver::getSubClass() const {
    return 0;
};

uint8_t PciDeviceDriver::getProgramInterface() const {
    return 0;
}