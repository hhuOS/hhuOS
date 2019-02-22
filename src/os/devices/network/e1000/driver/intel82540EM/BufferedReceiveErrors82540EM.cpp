#include <devices/cpu/Cpu.h>
#include "BufferedReceiveErrors82540EM.h"

BufferedReceiveErrors82540EM::BufferedReceiveErrors82540EM(uint8_t *address) : ReBufferedDefault(address) {}

bool BufferedReceiveErrors82540EM::symbolError() {
    return (bool) (errors & (1u << 1u));
}

bool BufferedReceiveErrors82540EM::sequenceError() {
    return (bool) (errors & (1u << 2u));
}

bool BufferedReceiveErrors82540EM::carrierExtensionError() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
    return false;
}

bool BufferedReceiveErrors82540EM::rxDataError() {
    return (bool) (errors & (1u << 7u));
}

bool BufferedReceiveErrors82540EM::hasErrors() {
    return crcOrAlignmentError()
           || symbolError()
           || sequenceError()
           || tcpUdpChecksumError()
           || ipChecksumError()
           || rxDataError();
}
