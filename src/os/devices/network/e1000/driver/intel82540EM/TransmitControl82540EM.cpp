#include "TransmitControl82540EM.h"

TransmitControl82540EM::TransmitControl82540EM(Register *request) : TcDefault(request) {
}

void TransmitControl82540EM::noRetransmitOnUnderrun(bool enable) {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}