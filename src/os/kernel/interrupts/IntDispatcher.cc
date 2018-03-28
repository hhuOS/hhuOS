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
#include <devices/Speaker.h>
#include <lib/Random.h>
#include "kernel/Cpu.h"

#include "kernel/interrupts/IntDispatcher.h"
#include "kernel/memory/SystemManagement.h"
#include "Pic.h"

extern "C" {
    void dispatchInterrupt(InterruptFrame *frame);
}

IOport systemA(0x92);
IOport systemB(0x61);

IOport slavePic(0xA0);
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
void dispatchInterrupt(InterruptFrame *frame) {

    IntDispatcher::getInstance().dispatch(frame);
}

IntDispatcher::IntDispatcher() : handler() {

}

IntDispatcher &IntDispatcher::getInstance() {

    static IntDispatcher instance;

    return instance;
}

void IntDispatcher::dispatch(InterruptFrame *frame) {

    // Extract interrupt information
    uint8_t slot = (uint8_t) frame->interrupt;
    uint32_t flags = frame->error;

    // Ignore spurious interrupts
    if (slot == 47 && Pic::isSpurious()) {

        return;
    }

    // Throw bluescreen on Protected Mode exceptions except pagefault
    if (slot < 32 && slot != (uint32_t) Cpu::Error::PAGE_FAULT) {
        Kernel::panic(frame);
    }

    // If this is a software exception, throw a bluescreen with error data
    if (slot >= Cpu::SOFTWARE_EXCEPTIONS_START) {
        Kernel::panic(frame);
    }

    if (slot == 14) {

        // Get page fault address and flags
        uint32_t faulting_address = 0;
        // the faulted linear address is loaded in the cr2 register
        asm ("mov %%cr2, %0" : "=r" (faulting_address));

        // There should be no access to the first page (address 0)
        if (faulting_address == 0) {
            frame->interrupt = (uint32_t) Cpu::Exception::NULLPOINTER;
            Kernel::panic(frame);
        }

        // Pass faulting address to the system management
        SystemManagement::getInstance()->setFaultParams(faulting_address, flags);
    }

    if (handler.size() == 0) {

        return;
    }

    Util::List<InterruptHandler*>* list = report(slot);

    if (list == nullptr) {

        return;
    }

    uint32_t size = list->size();

    for (uint32_t i = 0; i < size; i++) {

        list->get(i)->trigger();
    }

    if(slot > 32) {

        Pic::getInstance()->sendEOI(Pic::Interrupt(slot - 32));
    }
}

void IntDispatcher::assign(uint8_t slot, InterruptHandler &isr) {

    if (!handler.containsKey(slot)) {

        handler.put(slot, new Util::ArrayList<InterruptHandler*>);
    }

    handler.get(slot)->add(&isr);
}

Util::List<InterruptHandler*>* IntDispatcher::report(uint8_t slot) {

    if (!handler.containsKey(slot)) {

        return nullptr;
    }

    return handler.get(slot);
}




