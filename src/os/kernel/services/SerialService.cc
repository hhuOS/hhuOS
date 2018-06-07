#include "SerialService.h"

SerialService::SerialService() {
    serial = new Serial();
}

Serial *SerialService::getSerial() {
    return serial;
}
