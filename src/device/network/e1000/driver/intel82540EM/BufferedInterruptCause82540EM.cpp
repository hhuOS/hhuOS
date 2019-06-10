#include "device/cpu/Cpu.h"
#include "BufferedInterruptCause82540EM.h"

BufferedInterruptCause82540EM::BufferedInterruptCause82540EM(uint32_t *address) : IcBufferedDefault(address) {}

uint8_t BufferedInterruptCause82540EM::generalPurposeInterrupts_1() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
    return 0;
}

bool BufferedInterruptCause82540EM::hasReceiveSequenceError() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
    return false;
}

bool BufferedInterruptCause82540EM::hasReceivingCOrderedSets() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
    return false;
}

bool BufferedInterruptCause82540EM::isPhyInterrupt() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
    return false;
}

bool BufferedInterruptCause82540EM::isGeneralPurposeInterruptOnSDP6_2() {
    return processInterrupt(13u);
}

bool BufferedInterruptCause82540EM::isGeneralPurposeInterruptOnSDP7_3() {
    return processInterrupt(14u);
}

bool BufferedInterruptCause82540EM::isTransmitDescriptorLowThresholdHit() {
    return processInterrupt(15u);
}

bool BufferedInterruptCause82540EM::hasSmallReceivePacketDetected() {
    return processInterrupt(16u);
}