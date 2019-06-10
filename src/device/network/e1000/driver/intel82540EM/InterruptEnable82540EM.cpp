#include "device/cpu/Cpu.h"
#include "InterruptEnable82540EM.h"

InterruptEnable82540EM::InterruptEnable82540EM(Register *request) : IeDefault(request) {}

void InterruptEnable82540EM::receiveSequenceError() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptEnable82540EM::receivingCOrderedSets() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptEnable82540EM::phyInterrupt() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptEnable82540EM::generalPurposeInterrupts2(uint8_t value) {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptEnable82540EM::transmitDescriptorLowThresholdHit() {
    request->decide(1u << 15u, true);
}

void InterruptEnable82540EM::smallReceivePacketDetection() {
    request->decide(1u << 16u, true);
}