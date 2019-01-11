#include <devices/cpu/Cpu.h>
#include "Cmos.h"

IOport Cmos::registerPort(0x70);
IOport Cmos::dataPort(0x71);

bool Cmos::available = checkAvailability();

bool Cmos::isAvailable() {
    return available;
}

uint8_t Cmos::readRegister(uint8_t regNumber) {
    registerPort.outb(static_cast<uint8_t>(regNumber & 0x7f));

    return dataPort.inb();
}

void Cmos::writeRegister(uint8_t regNumber, uint8_t value) {
    registerPort.outb(static_cast<uint8_t>(regNumber & 0x7f));

    dataPort.outb(value);
}

void Cmos::disableNmi() {
    registerPort.outb(static_cast<uint8_t>(registerPort.inb() | 0x80));
}

void Cmos::enableNmi() {
    registerPort.outb(static_cast<uint8_t>(registerPort.inb() & 0x7Fu));
}

bool Cmos::checkAvailability() noexcept {
    Cpu::disableInterrupts();
    disableNmi();

    bool ret;

    uint8_t statusB = readRegister(0x0b);

    statusB ^= 0x03;

    writeRegister(0x0b, statusB);

    if(statusB == readRegister(0x0b)) {
        writeRegister(0x0b, static_cast<uint8_t>(statusB ^ 0x03));

        enableNmi();
        Cpu::enableInterrupts();

        return true;
    }

    enableNmi();
    Cpu::enableInterrupts();

    return false;
}
