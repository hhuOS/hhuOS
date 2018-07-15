/**
 * PIC - programmable interrupt controller. In this hardware device the different
 * interrupts can be activated or masked out. Using the PIC one can controll,
 * which hardware interrupts shall be passed to the CPU.
 *
 * @author  original by Olaf Spinczyk, TU Dortmund
 * 			modified by Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */

#include "kernel/interrupts/Pic.h"
#include "kernel/IOport.h"

static IOport PIC1_DATA(0x21);

static IOport PIC2_DATA(0xA1);

static IOport PIC1_COMMAND(0x20);

static IOport PIC2_COMMAND(0xA0);

Pic* Pic::instance = nullptr;

Pic *Pic::getInstance()  {

    if(instance == nullptr) {

        instance = new Pic();
    }

    return instance;
}

void Pic::allow(Pic::Interrupt interrupt) {

    IOport &port = getDataPort(interrupt);

    uint8_t mask = getMask(interrupt);

    port.outb(port.inb() & ~mask);
}

void Pic::forbid(Pic::Interrupt interrupt) {

    IOport &port = getDataPort(interrupt);

    uint8_t mask = getMask(interrupt);

    port.outb(port.inb() | mask);
}

bool Pic::status(Pic::Interrupt interrupt) {

    IOport &port = getDataPort(interrupt);

    uint8_t mask = getMask(interrupt);

    return port.inb() & mask;
}

void Pic::sendEOI(Pic::Interrupt interrupt) {
    if (interrupt >= Interrupt::RTC) {
        PIC2_COMMAND.outb(EOI);
    }

    PIC1_COMMAND.outb(EOI);
}

IOport& Pic::getDataPort(Pic::Interrupt interrupt) {

    if (interrupt >= Interrupt::RTC) {

        return PIC2_DATA;
    }

    return PIC1_DATA;
}

IOport& Pic::getCommandPort(Pic::Interrupt interrupt) {

    if (interrupt >= Interrupt::RTC) {

        return PIC2_COMMAND;
    }

    return PIC1_COMMAND;
}

uint8_t Pic::getMask(Pic::Interrupt interrupt) {

    if (interrupt >= Interrupt::RTC) {

        return (uint8_t ) (1 << ((uint8_t ) interrupt - 8));
    }

    return (uint8_t ) (1 << (uint8_t ) interrupt);
}

bool Pic::isSpurious() {
    PIC1_COMMAND.outb(READ_ISR);

    return (PIC1_COMMAND.inb() & SPURIOUS_INTERRUPT) == 0;
}
