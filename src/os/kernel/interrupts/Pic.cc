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

// different port numbers

static IOport IMR1 (0x21);    // interrupt mask register of PIC 1
static IOport IMR2 (0xA1);    // interrupt mask register of PIC 2

static IOport PIC1 (0x20);
static IOport PIC2 (0xA0);


Pic* Pic::pic = nullptr;


/**
 * Demasks an interrupt number in the corresponding PIC. If this is done,
 * all interrupts with this number will be passed to the CPU.
 */
void Pic::allow (int interrupt) {
    if (interrupt < 8)           // hardware-interrupt from PIC 1
        IMR1.outb (IMR1.inb () & (~(1 << interrupt)));
    else                         // hardware-Interrupt from PIC 2
        IMR2.outb (IMR2.inb () & (~(1 << (interrupt - 8))));
}


/**
 * Forbids an interrupt. If this is done, the interrupt is masked out
 * and every interrupts with this number that is thrown will be
 * surpressed and not arrive the CPU.
 */
void Pic::forbid (int interrupt) {
    if (interrupt < 8)           // hardware-interrupt from PIC 1
        IMR1.outb (IMR1.inb () | (1 << interrupt));
    else                         // hardware-interrupt from PIC 2
        IMR2.outb (IMR2.inb () | (1 << (interrupt - 8)));
}


/**
 * Gets the state of this interrupt - whether it is masked out or not.
 */
bool Pic::status (int interrupt) {
    if (interrupt < 8)           // hardware-interrupt from PIC 1
        return IMR1.inb () & (1 << interrupt);
    else                         // hardware-interrupt from PIC 2
        return IMR2.inb () & (1 << (interrupt - 8));
}

/**
 * Send an end of interrupt signal to the corresponding PIC.
 */
void Pic::sendEOI (unsigned int interrupt) {
    if (interrupt >= 8) {
        PIC2.outb(PIC_EOI);
    }

	PIC1.outb(PIC_EOI);
}
