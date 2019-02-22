
#include "NetworkService.h"


uint32_t NetworkService::getDriverCount() {
    return drivers.size();
}

E1000 *NetworkService::getDriver(uint8_t index) {
    return drivers.get(index);
}

void NetworkService::removeDriver(uint8_t index) {
    drivers.remove(index);
}

void NetworkService::registerDriver(E1000 *driver) {
    drivers.add(driver);
}
