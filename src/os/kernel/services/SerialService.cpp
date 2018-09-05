#include "SerialService.h"

Logger &SerialService::log = Logger::get("SERIAL");

SerialService::SerialService() {
    if(Serial::checkPort(Serial::COM1)) {
        log.info("Detected COM1");
        com1 = new Serial(Serial::COM1);
    }

    if(Serial::checkPort(Serial::COM2)) {
        log.info("Detected COM2");
        com2 = new Serial(Serial::COM2);
    }

    if(Serial::checkPort(Serial::COM3)) {
        log.info("Detected COM3");
        com3 = new Serial(Serial::COM3);
    }

    if(Serial::checkPort(Serial::COM4)) {
        log.info("Detected COM4");
        com4 = new Serial(Serial::COM4);
    }
}

Serial *SerialService::getSerialPort(Serial::ComPort port) {
    switch(port) {
        case Serial::COM1 :
            return com1;
        case Serial::COM2 :
            return com2;
        case Serial::COM3 :
            return com3;
        case Serial::COM4 :
            return com4;
        default:
            return nullptr;
    }
}

bool SerialService::isPortAvailable(Serial::ComPort port) {
    switch(port) {
        case Serial::COM1 :
            return com1 != nullptr;
        case Serial::COM2 :
            return com2 != nullptr;
        case Serial::COM3 :
            return com3 != nullptr;
        case Serial::COM4 :
            return com4 != nullptr;
        default:
            return false;
    }
}
