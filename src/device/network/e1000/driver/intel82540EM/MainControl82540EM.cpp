#include "device/cpu/Cpu.h"
#include "MainControl82540EM.h"

MainControl82540EM::MainControl82540EM(Register *request) : DcDefault(request) {}

MainControl82540EM::~MainControl82540EM() = default;

void MainControl82540EM::linkReset(bool enable) {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void MainControl82540EM::invertLossOfSignal(bool enable) {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void MainControl82540EM::wakeupAdvertisement(bool enable) {
    request->decide(1u << 20u, enable);
}

void MainControl82540EM::vlanMode(bool enable) {
    request->decide(1u << 30u, enable);
}

void MainControl82540EM::resetInternalPhy(bool enable) {
    request->decide(1u << 21u, enable);
}
