/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include <stdint.h>

#include "LocalApic.h"
#include "LocalApicErrorHandler.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/collection/Array.h"
#include "kernel/memory/GlobalDescriptorTable.h"

namespace Kernel {
enum GlobalSystemInterrupt : uint32_t;
enum InterruptVector : uint8_t;
}  // namespace Kernel

namespace Device {
class IoApic;
enum InterruptRequest : uint8_t;
class ApicTimer;

class Apic {

public:
    /**
     * Constructor.
     */
    Apic(const Util::Array<LocalApic*> &localApics, IoApic *ioApic);

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
     * Initialize the current processor's local APIC timer.
     */
    void startCurrentTimer();

    /**
     * Get the ApicTimer instance that belongs to the current CPU.
     */
    ApicTimer& getCurrentTimer();

    [[nodiscard]] bool isSymmetricMultiprocessingSupported() const;

    [[nodiscard]] uint8_t getCoreCount() const;
    
    void startupApplicationProcessors();

    Kernel::GlobalSystemInterrupt getIrqOverride(InterruptRequest interruptRequest);

    InterruptRequest getIrqSource(Kernel::GlobalSystemInterrupt gsi);

    Kernel::GlobalSystemInterrupt getMaxInterruptTarget();

    [[nodiscard]] const Util::Array<LocalApic*>& getLocalApics() const;
    
private:

    /**
     * Prepare the memory regions used by the AP's stacks.
     *
     * @return The virtual address of the stackpointer array
     */
    uint8_t** prepareApplicationProcessorStacks();

    /**
     * Copy the AP startup routine to lower physical memory.
     *
     * Because this memory is identity-mapped, the physical address can be used to free the memory again.
     *
     * @return The virtual/physical address at which the startup routine is located
     */
    void prepareApplicationProcessorStartupCode(void *gdts, void *stacks);

    /**
     * Place the AP startup routine address into the warm reset vector and prepare CMOS for warm reset.
     *
     * @return The virtual address of the allocated memory used to write the warm-reset vector
     */
    void prepareApplicationProcessorWarmReset();

    Kernel::GlobalDescriptorTable::Descriptor** prepareApplicationProcessorGdts();

    static Util::Array<LocalApic*> initializeLocalApics();

    static IoApic* initializeIoApic();

    // Memory allocated for or by instances contained in these lists is never freed,
    // this implementation doesn't support disabling the APIC at all.
    // Once the switch from PIC to APIC is done, it can't be switched back.
    Util::Array<LocalApic*> localApics;  // All LocalApic instances.
    IoApic *ioApic;                      // The IoApic instance responsible for the external interrupts.
    LocalApicErrorHandler errorHandler;  // The interrupt handler that gets triggered on an internal APIC error.

};

}

#endif
