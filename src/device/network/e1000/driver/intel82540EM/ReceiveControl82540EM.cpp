#include "ReceiveControl82540EM.h"

ReceiveControl82540EM::ReceiveControl82540EM(Register *request) : RcDefault(request) {}

void ReceiveControl82540EM::loopbackMode(uint8_t value) {
    if ((value > 3u) | (value == 2u) | (value == 1u)) {
        Cpu::throwException(Cpu::Exception::INVALID_ARGUMENT, "Inappropriate loopback Mode");
    }
    request->set(value << 6u, 3u << 6u);
}

void ReceiveControl82540EM::vlanFilter(bool enable) {
    request->decide(1u << 18u, enable);
}