/**
 * Kernel - provides global access to kernel services so that they are available
 * to all applications.
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */

#include <devices/graphics/text/CgaText.h>
#include <lib/libc/printf.h>
#include "Kernel.h"
#include "KernelSymbols.h"
#include "kernel/Cpu.h"

Spinlock Kernel::serviceLock;

Util::HashMap<String, KernelService*> Kernel::serviceMap(SERVICE_MAP_SIZE);

/**
 * Returns the kernel service that is registered with a given ID.
 */
KernelService* Kernel::getService(const String &serviceId) {
    return serviceMap.get(serviceId);
}

/**
 * Registers an instance of a kernel service under a given ID.
 */
void Kernel::registerService(const String &serviceId, KernelService* const &kernelService) {
    serviceLock.lock();
    serviceMap.put(serviceId, kernelService);
    serviceLock.unlock();
}

bool Kernel::isServiceRegistered(const String &serviceId) {

    return serviceMap.containsKey(serviceId);
}

/**
 * Triggers a kernel panic with the given interrupt frame
 */
void Kernel::panic(InterruptFrame *frame) {

    Cpu::disableInterrupts();
    uint32_t slot = frame->interrupt;

    // initialize a CGA text for panic because it is the
    // most save way for outprints
    CgaText cgaText;
    cgaText.init(80, 25, 4);

    // Paint screen blue
    auto *dest = (uint64_t *) VIRT_CGA_START;
    uint64_t end = cgaText.getRowCount() * cgaText.getColumnCount() * 2 / sizeof(uint64_t);

    for(uint64_t i = 0; i < end; i++) {
        dest[i] = 0x1000100010001000;
    }

    cgaText.setpos(0, 0);

    stdout = &cgaText;

    // print out the "bluescreen"

    printf("\n\n  [PANIC] %s\n\n", Cpu::getExceptionName(slot));

    printStacktrace(frame->ebp, frame->eip);

    printRegisters(*frame);

    // do not execute any further
    Cpu::halt();
}

/**
 * Prints the stack trace for a kernel panic.
 */
void Kernel::printStacktrace(uint32_t basePointer, uint32_t instructionPointer, uint32_t skip) {

        uint32_t *ebp = (uint32_t*) basePointer;
        uint32_t eip = instructionPointer;
        uint32_t i = 0;

        while (skip) {

            eip = ebp[1];

            ebp = (uint32_t*) ebp[0];

            skip--;
        }

        while (eip) {

            printf("     #%02d 0x%08x --- %s\n", i, eip, (char*) KernelSymbols::get(eip));

            eip = ebp[1];

            ebp = (uint32_t*) ebp[0];

            if ((uint32_t ) ebp < KERNEL_START) {
                break;
            }

            i++;
        }
}

/**
 * Print state of registers in given interrupt frame
 */
void Kernel::printRegisters(const InterruptFrame &frame) {

    printf("\n\n");
    printf("     eax=0x%08x  ebx=0x%08x  ecx=0x%08x  edx=0x%08x\n", frame.eax, frame.ebx, frame.ecx, frame.edx);
    printf("     esp=0x%08x  ebp=0x%08x  esi=0x%08x  edi=0x%08x\n\n", frame.esp, frame.ebp, frame.esi, frame.edi);
    printf("     eflags=0x%08x", frame.eflags);
}
