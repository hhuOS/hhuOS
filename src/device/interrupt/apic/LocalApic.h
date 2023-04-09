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

#ifndef HHUOS_LOCALAPIC_H
#define HHUOS_LOCALAPIC_H

#include <cstdint>

#include "kernel/interrupt/InterruptVector.h"
#include "lib/util/collection/Array.h"
#include "lib/util/async/Spinlock.h"
#include "kernel/log/Logger.h"
#include "device/cpu/ModelSpecificRegister.h"
#include "lib/util/collection/ArrayList.h"

namespace Device {

class LocalApic {

public:
    /**
     * Lists the local APIC's local interrupts.
     *
     * Every individual local APIC has these, they are completely separate from
     * the usual (PIC and I/O APIC) hardware interrupt inputs.
     */
    enum LocalInterrupt : uint8_t {
        CMCI = 0,       // Might not exist
        TIMER = 1,      // The APIC timer local interrupt
        THERMAL = 2,
        PERFORMANCE = 3,
        LINT0 = 4,      // Local interrupt 0, used in virtual wire mode
        LINT1 = 5,      // Local interrupt 1, used as NMI source
        ERROR = 6       // The APIC error interrupt
    };

    /**
     * Information obtainable from the local vector table of the current CPU's local APIC.
     *
     * Affects handling of local interrupts.
     */
    struct LocalVectorTableEntry {
        enum class DeliveryMode : uint8_t {
            FIXED = 0,
            SMI = 0b10,
            NMI = 0b100,
            INIT = 0b101,
            EXTINT = 0b111
        };
        enum class DeliveryStatus : uint8_t {
            IDLE = 0,
            PENDING = 1
        };
        enum class PinPolarity : uint8_t {
            HIGH = 0,
            LOW = 1
        };
        enum class TriggerMode : uint8_t {
            EDGE = 0,
            LEVEL = 1
        };
        enum class TimerMode : uint8_t {
            ONESHOT = 0,
            PERIODIC = 1
        };

        Kernel::InterruptVector vector;
        DeliveryMode deliveryMode;     // All except timer
        DeliveryStatus deliveryStatus; // RO
        PinPolarity pinPolarity;       // Only LINT0, LINT1
        TriggerMode triggerMode;       // Only LINT0, LINT1
        bool isMasked;
        TimerMode timerMode; // Only timer

        LocalVectorTableEntry() = default;

        explicit LocalVectorTableEntry(uint32_t registerValue);

        explicit operator uint32_t() const;
    };

    /**
     * Lists the offsets, relative to the APIC base address, for MMIO register access.
     *
     * Described in the IA-32 manual, sec. 3.11.4.1
     */
    enum Register : uint16_t {
        ID = 0x20,        // Local APIC id, in SMP systems the id is used as the CPU id
        VER = 0x30,       // Local APIC version
        TPR = 0x80,       // Task Priority Register
        APR = 0x90,       // Arbitration Priority Register
        PPR = 0xA0,       // Processor Priority Register
        EOI = 0xB0,       // End-of-Interrupt Register
        RRD = 0xC0,       // Remote Read Register
        LDR = 0xD0,       // Logical Destination Register
        DFR = 0xE0,       // Destination Format Register
        SVR = 0xF0,       // Spurious Interrupt Vector Register
        ISR = 0x100,      // In-Service Register (255 bit)
        TMR = 0x180,      // Trigger Mode Register (255 bit)
        IRR = 0x200,      // Interrupt Request Register (255 bit)
        ESR = 0x280,      // Error Status Register
        ICR_LOW = 0x300,  // Interrupt Command Register (lower 32 bit)
        ICR_HIGH = 0x310, // Interrupt Command Register (upper 32 bit)

        // These are located here, instead of in the ApicTimer class, because this class does the register access
        TIMER_INITIAL = 0x380, // Timer Initial Count Register
        TIMER_CURRENT = 0x390, // Timer Current Count Register
        TIMER_DIVIDE = 0x3E0   // Timer Divide Configuration Register
    };

    /**
     * Information obtainable from the local APIC's model specific register.
     */
    struct BaseModelSpecificRegisterEntry {
        bool isBootstrapProcessor;
        bool isX2Apic;
        bool isXApic;
        uint32_t baseField;

        BaseModelSpecificRegisterEntry() = default;

        explicit BaseModelSpecificRegisterEntry(uint64_t registerValue);

        explicit operator uint64_t() const;
    };

    /**
     * Information obtainable from the spurious interrupt vector register of the current CPU's local APIC.
     */
    struct SpuriousVectorRegisterEntry {
        Kernel::InterruptVector vector;
        bool isSwEnabled;
        bool hasFocusProcessorChecking;
        bool suppressEoiBroadcasting;

        SpuriousVectorRegisterEntry() = default;

        explicit SpuriousVectorRegisterEntry(uint32_t registerValue);

        explicit operator uint32_t() const;
    };

    /**
     * Information obtainable from the interrupt command register of the current CPU's local APIC.
     *
     * Affects what interprocessor interrupt is issued.
     */
    struct InterruptCommandRegisterEntry {
        enum class DeliveryMode : uint8_t {
            FIXED = 0,
            LOWPRIO = 1, // Model specific
            SMI = 0b10,
            NMI = 0b100,
            INIT = 0b101,
            STARTUP = 0b110
        };
        enum class DestinationMode : uint8_t {
            PHYSICAL = 0,
            LOGICAL = 1
        };
        enum class DeliveryStatus : uint8_t {
            IDLE = 0,
            PENDING = 1
        };
        enum class Level : uint8_t {
            DEASSERT = 0,
            ASSERT = 1
        };
        enum class TriggerMode : uint8_t {
            EDGE = 0,
            LEVEL = 1
        };
        enum class DestinationShorthand : uint8_t { // If used ICR_DESTINATION_FIELD is ignored
            NO = 0,
            SELF = 1,
            ALL = 0b10,
            ALL_NO_SELF = 0b11
        };

        Kernel::InterruptVector vector;
        DeliveryMode deliveryMode;
        DestinationMode destinationMode;
        DeliveryStatus deliveryStatus; // RO
        Level level;
        TriggerMode triggerMode;
        DestinationShorthand destinationShorthand;
        uint8_t destination;

        InterruptCommandRegisterEntry() = default;

        explicit InterruptCommandRegisterEntry(uint64_t registerValue);

        explicit operator uint64_t() const;
    };

    struct NmiSource {
        LocalInterrupt source;                       // The local interrupt pin that acts as NMI source.
        LocalVectorTableEntry::PinPolarity polarity; // The NMI source's pin polarity.
        LocalVectorTableEntry::TriggerMode trigger;  // The NMI source's trigger mode.

        bool operator!=(const NmiSource &other) const;
    };

    /**
     * Constructor.
     */
    explicit LocalApic(uint8_t cpuId);

    /**
     * Copy Constructor.
     */
    LocalApic(const LocalApic &other) = delete;

    /**
     * Assignment operator.
     */
    LocalApic &operator=(const LocalApic &other) = delete;

    /**
     * Destructor.
     */
    ~LocalApic() = default;

    /**
     * Check if the local APIC supports xApic mode (xApic uses MMIO-based register access)
     *
     * Determined using CPUID.
     */
    [[nodiscard]] static bool supportsXApic() ;

    /**
     * Check if the local APIC supports x2Apic mode (x2Apic uses MSR-based register access)
     *
     * Determined using CPUID.
     */
    [[nodiscard]] static bool supportsX2Apic() ;

    /**
     * Get the id of the local APIC belonging to the current CPU.
     *
     * Can be used to determine what CPU is currently executing the calling code in SMP systems.
     * To get the id of a LocalApic instance, use the "cpuId" field.
     */
    [[nodiscard]] static uint8_t getId();

    /**
     * Determine the local APIC version
     */
    [[nodiscard]] static uint8_t getVersion();

    /**
     * Initialize LVT, SVR and TPR of the executing core's local APIC.
     *
     * The local APIC initialization consists of multiple steps:
     * 1. The bootstrap processor calls LocalApic::enableXApicMode(), to set up the system for local APIC initialization.
     * 2. The bootstrap processor calls LocalApic::enable(), to complete the its local APIC initialization.
     * 3. The APs are booted up.
     * 4. Every AP calls LocalApic::enable() individually.
     *
     * This function must not be called before LocalApic::enableXApicMode().
     */
    void initialize();

    static void synchronizeArbitrationIds();

    /**
     * Unmask a local interrupt in the local APIC of the current CPU.
     *
     * @param lint The local interrupt to activate
     */
    static void allow(LocalInterrupt lint);

    /**
     * Forbid a local interrupt in the local APIC of the current CPU.
     *
     * @param lint The local interrupt to deactivate
     */
    static void forbid(LocalInterrupt lint);

    /**
     * Get the state of this interrupt - whether it is masked out or not.
     *
     * @param lint The local interrupt
     * @return True, if the interrupt is disabled in the local APIC of the current CPU
     */
    static bool status(LocalInterrupt lint);

    /**
     * Send an end-of-interrupt signal to the local APIC of the current CPU.
     *
     * The signal will be broadcasted to all I/O APICs if the interrupt was level-triggered and broadcasting
     * is enabled in the SVR (IA-32 manual, sec. 3.11.8.5), as for level-triggered interrupts servicing
     * completion has to be signaled to both, the local APIC and the I/O APIC(s).
     * Edge-triggered interrupts are only EOI'd to the local APIC.
     */
    static void sendEndOfInterrupt();

    /**
     * Clear the local APIC error register of the current CPU.
     */
    static void clearErrors();

    /**
     * Send an INIT IPI to an AP.
     *
     * The INIT IPI is used for discrete APICs. It resets the AP to start executing
     * BIOS code. If a warm reset is performed while the startup routine's address
     * is set in the warm reset vector, it can be used to boot an AP.
     *
     * @param id The local APIC id/CPU id of the AP to enable
     * @param level Assert or deassert
     */
    static void sendInitInterProcessorInterrupt(uint8_t id, InterruptCommandRegisterEntry::Level level);

    /**
     * Send an STARTUP IPI (SIPI) to an AP.
     *
     * The STARTUP IPI is used by the xApic architecture, it instructs an AP to load
     * its startup routine from a supplied address and execute it, booting the AP.
     *
     * @param id The local APIC id/CPU id of the AP to boot
     * @param startupCodeAddress The page on which the startup routine is located in physical memory
     */
    static void sendStartupInterProcessorInterrupt(uint8_t id, uint32_t startupCodeAddress);

    /**
     * Poll the ICR until the delivery status bit is unset.
     */
    static void waitForInterProcessorInterruptDispatch();

    /**
     * Initialize the local APIC's local vector table.
     *
     * Marks every local interrupt in the local vector table as edge-triggered,
     * active high, masked and fixed delivery mode.
     * Vector numbers are set to InterruptVector equivalents.
     */
    static void initializeLocalVectorTable();

    void addNonMaskableInterrupt(LocalInterrupt nmiLint, LocalVectorTableEntry::PinPolarity nmiPolarity, LocalVectorTableEntry::TriggerMode nmiTrigger);

    /**
     * Read the IA32_APIC_BASE_MSR.
     *
     * IA-32 manual, sec. 3.11.12.1 and 4.1
     */
    [[nodiscard]] static BaseModelSpecificRegisterEntry readBaseModelSpecificRegister();

    /**
     * Write the IA32_APIC_BASE_MSR.
     *
     * IA-32 manual, sec. 3.11.12.1 and 4.1
     */
    static void writeBaseModelSpecificRegister(const BaseModelSpecificRegisterEntry &msrEntry);

    /**
     * Read the spurious interrupt vector register.
     *
     * IA-32 manual, sec. 3.11.9
     */
    [[nodiscard]] static SpuriousVectorRegisterEntry readSpuriousVectorRegister();

    /**
     * Write the spurious interrupt vector register.
     *
     * IA-32 manual, sec. 3.11.9
     */
    static void writeSpuriousVectorRegister(const SpuriousVectorRegisterEntry &svrEntry);

    /**
     * Read a local vector table register, identified by the local interrupt.
     *
     * IA-32 manual, sec. 3.11.5.1
     */
    [[nodiscard]] static LocalVectorTableEntry readLocalVectorTable(LocalInterrupt lint);

    /**
     * Write a local vector table register, identified by the local interrupt.
     *
     * IA-32 manual, sec. 3.11.5.1
     */
    static void writeLocalVectorTable(LocalInterrupt lint, const LocalVectorTableEntry &lvtEntry);

    /**
     * Read the interrupt command register.
     *
     * IA-32 manual, sec. 3.11.6.1
     */
    [[nodiscard]] static InterruptCommandRegisterEntry readInterruptCommandRegister(); // Obtain delivery status of IPI

    /**
     * Write the interrupt command register.
     *
     * IA-32 manual, sec. 3.11.6.1
     */
    static void writeInterruptCommandRegister(const InterruptCommandRegisterEntry &icrEntry); // Issue IPIs

    /**
     * Prepare the BSP for local APIC initialization.
     *
     * Only has to be called once, not once per AP.
     * This implementation only supports xApic mode. Because the local APIC starts with xApic mode and every AP uses
     * the same address space, memory allocation only has to be done once and the IA32_APIC_BASE MSR does not
     * have to be written at all. To enable x2Apic mode, every AP would have to set the x2Apic-enable flag in its
     * IA32_APIC_BASE MSR, without requiring the MMIO region.
     */
    static void enableXApicMode(uint32_t baseAddress);

    /**
     * Read a 32-bit register identified by a memory offset relative to the APIC base address.
     */
    [[nodiscard]] static uint32_t readDoubleWord(Register reg);

    /**
     * Write a 32-bit register identified by a memory offset relative to the APIC base address.
     */
    static void writeDoubleWord(Register reg, uint32_t value);

    [[nodiscard]] uint8_t getCpuId() const;

private:

    uint8_t cpuId; // The CPU core this instance belongs to, LocalApic::getId() only returns the current AP's id!
    Util::ArrayList<NmiSource> nmiSources;

    static uint32_t mmioAddress; // The virtual address used to access registers in xApic mode.

    static ModelSpecificRegister ia32ApicBaseMsr; // Core unique MSR (every core can only address its own MSR).
    static Util::Array<Register> lintRegs;

    static Util::Async::Spinlock commandLock;
};

}

#endif
