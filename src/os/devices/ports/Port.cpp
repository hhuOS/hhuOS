#include "Port.h"

void Port::sendData(char *data, uint32_t len) {
    for(uint32_t i = 0; i < len; i++) {
        sendChar(data[i]);
    }
}

void Port::readData(char *data, uint32_t len) {
    for(uint32_t i = 0; i < len; i++) {
        data[i] = readChar();
    }
}
