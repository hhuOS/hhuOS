#include "device/cpu/Cpu.h"
#include "InterruptDisable82540EM.h"

InterruptDisable82540EM::InterruptDisable82540EM(Register *request) : IdDefault(request) {}

void InterruptDisable82540EM::receiveSequenceError() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptDisable82540EM::receivingCOrderedSets() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptDisable82540EM::phyInterrupt() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptDisable82540EM::generalPurposeInterrupts2(uint8_t value) {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptDisable82540EM::transmitDescriptorLowThresholdHit() {
    request->decide(1u << 15u, true);
}

void InterruptDisable82540EM::smallReceivePacketDetection() {
    request->decide(1u << 16u, true);
}

void InterruptDisable82540EM::clearReserved() {
    request->decide(1u << 3u, true);
    request->decide(1u << 5u, true);
    request->decide(1u << 8u, true);
    request->decide(1u << 10u, true);
    request->decide(1u << 11u, true);
    request->decide(1u << 12u, true);
    request->decide(0xFFFEu, true);
}