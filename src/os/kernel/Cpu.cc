/**
 * CPU - Provides abstraction and functionality around the CPU. Interrupts
 * can be enabled and disabled here and exceptions be handled.
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */

#include <lib/String.h>
#include "Cpu.h"

extern "C" {
    void onException(uint32_t exception);
}

// lists of Exceptions that cann occur
const char* Cpu::hardwareExceptions[] = {
        "Divide-by-zero Error", "Debug", "Non-maskable Interrupt", "Breakpoint",
        "Overflow", "Bound Range Exceeded", "Invalid Opcode", "Device not available",
        "Double Fault", "Coprocessor Segment Overrun", "Invalid TSS", "Segment Not Present",
        "Stack-Segment Fault", "General Protection Fault", "Page Fault", "Reserved",
        "x87 Floating-Point Exception", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception",
        "Virtualization Exception", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
        "Reserved", "Reserved", "Reserved", "Security Exception", "Reserved"
};
const char* Cpu::softwareExceptions[] {
        "NullPointer Exception", "IndexOutOfBounds Exception", "InvalidArgument Exception", "KeyNotFound Exception"
};

// enabling and disabling interrupts is done in assembler code
extern "C" {
    void enable_interrupts();
    void disable_interrupts();
};

/**
 * Checks if interrupt flag is set in EFLAGS.
 */
bool Cpu::isInterrupted() {

    uint32_t eflags;

    asm volatile (  "pushf\n"
                    "pop %0" : "=a"(eflags) );

    return (eflags & 0x200) == 0;
}

/**
 * Enables hardware interrupts on CPU.
 */
void Cpu::enableInterrupts() {
    asm volatile ( "sti" );
}

/**
 * Disables hardware interrupts on CPU.
 */
void Cpu::disableInterrupts() {
    asm volatile ( "cli" );
}

/**
 * Stop CPU unitl next interrupt.
 */
void Cpu::idle () {
    asm volatile ( "sti\n"
                   "hlt"
    );
}

/**
 * Stop the processor via hlt instruction.
 */
void Cpu::halt () {
    asm volatile ( "cli\n"
                   "hlt"
    );
    __builtin_unreachable();
}

/**
 * Reads the time stamp counter.
 */
unsigned long long int Cpu::rdtsc() {
    unsigned long long int  ret;
    asm volatile ( "rdtsc" : "=A"(ret) );
    return ret;
}

/**
 * Returns the name of the Exception from the ennumeration.
 */
const char *Cpu::getExceptionName(Cpu::Error exception) {

    uint32_t slot = (uint32_t) exception;

    if (slot >= SOFTWARE_EXCEPTIONS_START) {
        return softwareExceptions[slot - SOFTWARE_EXCEPTIONS_START];
    }

    return hardwareExceptions[slot];
}


const char *Cpu::getExceptionName(uint32_t exception) {

    if (exception >= SOFTWARE_EXCEPTIONS_START) {
        return softwareExceptions[exception - SOFTWARE_EXCEPTIONS_START];
    }

    return hardwareExceptions[exception];
}

/**
 * Throws an exception.
 */
void Cpu::throwException(Exception exception) {

    disableInterrupts();

    onException((uint32_t) exception);
}
