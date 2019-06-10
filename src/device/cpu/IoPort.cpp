#include "IoPort.h"

IoPort::IoPort(uint16_t a) noexcept : address(a) {

};

void IoPort::outb(uint8_t val) const {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(address) );
}

void IoPort::outb(uint16_t addr_offset, uint8_t val) const {
    uint16_t addr = address + addr_offset;
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(addr) );
}

void IoPort::outw(uint16_t val) const {
    asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(address) );
}

void IoPort::outdw(uint32_t val) const {
    asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(address) );
}

uint8_t IoPort::inb() const {
    uint8_t ret;

    asm volatile ( "inb %1, %0"
    : "=a"(ret)
    : "Nd"(address) );
    return ret;
}

uint8_t IoPort::inb(uint16_t addr_offset) const {
    uint8_t ret;
    uint16_t addr = address + addr_offset;
    asm volatile ( "inb %1, %0"
    : "=a"(ret)
    : "Nd"(addr) );
    return ret;
}

uint16_t IoPort::inw() const {
    uint16_t ret;

    asm volatile ( "inw %1, %0"
    : "=a"(ret)
    : "Nd"(address) );
    return ret;
}

uint32_t IoPort::indw() const {
    uint32_t ret;

    asm volatile ( "inl %1, %0"
    : "=a"(ret)
    : "Nd"(address) );
    return ret;
}

uint16_t IoPort::getAddress() {
    return address;
}