/**
 * IntDispatcher - responsible for registering and dispatching interrupts to the
 * corresponding handlers.
 *
 * @author Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */

#include <kernel/Kernel.h>
#include <kernel/KernelSymbols.h>
#include <kernel/IOport.h>
#include <lib/libc/printf.h>
#include "kernel/Cpu.h"

#include "kernel/threads/ThreadState.h"
#include "kernel/interrupts/IntDispatcher.h"
#include "../memory/SystemManagement.h"

extern "C" {
    void int_disp (InterruptFrame *frame);
}

// map with interrupt handlers
InterruptHandler* IntDispatcher::map[MAP_SIZE_X][MAP_SIZE_Y];

IOport systemA(0x92);
IOport systemB(0x61);

IOport slavePic(0xa0);
IOport masterPic(0x20);

void printNMI() {
    printf("\n");
    printf("  System Control Port A :       %b\n", systemA.inb());
    printf("  System Control Port B :       %b\n", systemB.inb());
}

/** Central function to deal with interrupts and faults - is called from assembler code
 *
 * @param *frame - pointer to the interrupt frame containing all relevant data
 */
void int_disp (InterruptFrame *frame) {

	// extract interrupt information
    uint32_t slot = frame->interrupt;
    uint32_t flags = frame->error;

    // throw bluescreen on Protected Mode exceptions except pagefault
    if (slot < 32 && slot != (uint32_t) Cpu::Error::PAGE_FAULT) {
        Kernel::panic(frame);
    }

    // if this is a software exception, throw a bluescreen with error data
    if (slot >= Cpu::SOFTWARE_EXCEPTIONS_START) {
        Kernel::panic(frame);
    }

    // TODO:
    //  Handle primary and secondary IDE/ATA
    if (slot == 47 || slot == 48) {
        return;
    }

    if (slot == 14) {

        // get page fault address and flags
        uint32_t faulting_address = 0;
        // the faulted linear address is loaded in the cr2 register
        asm ("mov %%cr2, %0" : "=r" (faulting_address));

        // there should be no excess to the first page (address 0)
        if (faulting_address == 0) {
            frame->interrupt = (uint32_t) Cpu::Exception ::NULLPOINTER;
            Kernel::panic(frame);
        }

        // pass faulting address to the system management
        SystemManagement::getInstance()->setFaultParams(faulting_address, flags);
    }

    // pointer to interrupt handler
    InterruptHandler* isr;
    // count for interrupt-handler devices
    uint8_t device;
    // iterate through all devices registered to handle this interrupt number
    // and trigger them to deal with the interrupt
    for (device = 0; device < 16; device++) {
        isr = IntDispatcher::report(slot, device);

        if (isr == 0x0) {
            break;
        }

        isr->trigger();
    }

    // For some weird reason, we need to send an EOI to both PICs, when the RTC triggers.
    // Otherwise, the RTC won't work on real hardware.
    if(device > 0 && slot == 40) {
        masterPic.outb(0x20);
        slavePic.outb(0x20);
    }

    if (device == 0) {
//        printf("[WARNING] Received unexpected interrupt %d\n", slot);
    }
}


/**
 * Register an interrupt handler to an interrupt number.
 */
void IntDispatcher::assign (uint8_t slot, InterruptHandler &isr) {
	// search for the next free device number for this interrupt and register the handler
    if (slot < MAP_SIZE_X) {
        for (uint8_t i = 0; i < MAP_SIZE_Y; i++) {
            if ( map[slot][i] == 0x0 ) {
                map[slot][i] = &isr;
                break;
            }
        }
    }
}


/**
 * Get the interrutp handler that is registered for an interrupt number
 * under the given device number.
 */
InterruptHandler* IntDispatcher::report (uint8_t slot, uint8_t device) {
    if ( slot < MAP_SIZE_X && device < MAP_SIZE_Y ) {
        return map[slot][device];
    } else {
        return nullptr;
    }
}
