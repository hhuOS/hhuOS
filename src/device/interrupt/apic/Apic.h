/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * The APIC implementation is based on a bachelor's thesis, written by Christoph Urlacher.
 * The original source code can be found here: https://github.com/ChUrl/hhuOS
 */

#ifndef HHUOS_APIC_H
#define HHUOS_APIC_H

#include <cstdint>
#include "LocalApic.h"
#include "IoApic.h"
#include "LocalApicErrorHandler.h"
#include "device/interrupt/InterruptRequest.h"
#include "lib/util/collection/ArrayList.h"
#include "device/time/ApicTimer.h"
#include "device/cpu/Cpu.h"
#include "lib/util/collection/HashMap.h"

namespace Device {

class Apic {

public:
    /**
     * Constructor.
     */
    Apic(const Util::Array<LocalApic*> &localApicsArray, IoApic *ioApic);

    /**
     * Copy Constructor.
     */
    Apic(const Apic &other) = delete;

    /**
     * Assignment operator.
     */
    Apic &operator=(const Apic &other) = delete;

    /**
     * Destructor.
     */
    ~Apic() = default;

    /**
     * Check if the system supports the APIC interrupt architecture.
     */
    static bool isAvailable();

    /**
     * Initialize the bootstrap processor's local APIC and all I/O APICs.
     *
     * Includes APIC timer and APIC error handler.
     */
    static Apic* initialize();

    void initializeCurrentLocalApic();

    /**
     * Get the LocalApic instance that belongs to the current CPU.
     */
    LocalApic& getCurrentLocalApic();

    /**
     * Create an ErrorHandler instance (if it hasn't been created yet) and allow the local ERROR interrupt.
     */
    void enableCurrentErrorHandler();

    /**
     * Unmask an external interrupt for the current CPU.
     */
    void allow(InterruptRequest interruptRequest);

    /**
     * Mask an external interrupt for the current CPU.
     */
    void forbid(InterruptRequest interruptRequest);

    /**
     * Check if an external interrupt is masked or unmasked for the current CPU.
     *
     * @return True, if the interrupt is masked.
     */
    bool status(InterruptRequest interruptRequest);

    /**
     * Signal the completion of an interrupt to the current CPU, local or external.
     */
    void sendEndOfInterrupt(Kernel::InterruptVector vector);

    /**
     * Check if an interrupt vector belongs to a local interrupt (Local APIC).
     */
    bool isLocalInterrupt(Kernel::InterruptVector vector) const;

    /**
     * Check if an interrupt vector belongs to an external hardware interrupt (I/O APIC).
     */
    bool isExternalInterrupt(Kernel::InterruptVector vector) const;

    /**
     * Check if this core's local APIC timer has been initialized.
     */
    bool isCurrentTimerRunning();

    /**
     * Initialize the current processor's local APIC timer.
     */
    void startCurrentTimer();

    /**
     * Get the ApicTimer instance that belongs to the current CPU.
     */
    ApicTimer &getCurrentTimer();

    [[nodiscard]] bool isSymmetricMultiprocessingSupported() const;
    
    void startupApplicationProcessors();
    
private:

    /**
     * Prepare the memory regions used by the AP's stacks.
     *
     * @return The virtual address of the stackpointer array
     */
    void* prepareApplicationProcessorStacks();

    /**
     * Copy the AP startup routine to lower physical memory.
     *
     * Because this memory is identity-mapped, the physical address can be used to free the memory again.
     *
     * @return The virtual/physical address at which the startup routine is located
     */
    void* prepareApplicationProcessorStartupCode(void *gdts, void *stacks);

    /**
     * Place the AP startup routine address into the warm reset vector and prepare CMOS for warm reset.
     *
     * @return The virtual address of the allocated memory used to write the warm-reset vector
     */
    void* prepareApplicationProcessorWarmReset();

    void* prepareApplicationProcessorGdts();

    /**
     * Sets up the GDT for the AP.
     *
     * The memory is allocated by MemoryService::AllocateKernelMemory with enabled paging, so its a virtual address.
     * This is basically a shorter and slightly modified version of System::InitializeGlobalDescriptorTables.
     * The main difference is that only a single GDT is used and its memory is allocated by this function.
     */
    Cpu::Descriptor* allocateApplicationProcessorGdt();

    Kernel::GlobalSystemInterrupt getIrqOverride(InterruptRequest interruptRequest);

    static Util::Array<LocalApic*> getLocalApics();

    static IoApic* getIoApic();

    // Memory allocated for or by instances contained in these lists is never freed,
    // this implementation doesn't support disabling the APIC at all.
    // Once the switch from PIC to APIC is done, it can't be switched back.
    Util::HashMap<uint8_t, LocalApic*> localApics;  // All LocalApic instances.
    Util::HashMap<uint8_t, ApicTimer*> localTimers; // All ApicTimer instances.
    IoApic *ioApic;                      // The IoApic instance responsible for the external interrupts.
    LocalApicErrorHandler errorHandler;  // The interrupt handler that gets triggered on an internal APIC error.

    static Kernel::Logger log;

};

}

#endif
