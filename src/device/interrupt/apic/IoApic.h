/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_IOAPIC_H
#define HHUOS_IOAPIC_H

#include <stdint.h>

#include "lib/util/async/Spinlock.h"
#include "lib/util/collection/ArrayList.h"

namespace Kernel {
enum GlobalSystemInterrupt : uint32_t;
enum InterruptVector : uint8_t;
}  // namespace Kernel

namespace Device {
enum InterruptRequest : uint8_t;

class IoApic {

public:
    /**
     * Information obtainable from the redirection table of an IO APIC.
     *
     * Affects handling of external interrupts.
     */
    struct RedirectionTableEntry {
        enum class DeliveryMode : uint8_t {
            FIXED = 0,
            LOWPRIO = 1,
            SMI = 0b10,
            NMI = 0b100,
            INIT = 0b101,
            EXTINT = 0b111
        };
        enum class DestinationMode : uint8_t {
            PHYSICAL = 0,
            LOGICAL = 1
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

        Kernel::InterruptVector vector;
        DeliveryMode deliveryMode;
        DestinationMode destinationMode;
        DeliveryStatus deliveryStatus; // RO
        PinPolarity pinPolarity;
        TriggerMode triggerMode;
        bool isMasked;
        uint8_t destination;

        RedirectionTableEntry() = default;

        explicit RedirectionTableEntry(uint64_t registerValue);

        explicit operator uint64_t() const;
    };

    struct NmiSource {
        Kernel::GlobalSystemInterrupt source;
        RedirectionTableEntry::PinPolarity polarity;
        RedirectionTableEntry::TriggerMode trigger;

        bool operator!=(const NmiSource &other) const;
    };

    struct IrqOverride {
        InterruptRequest source;
        Kernel::GlobalSystemInterrupt target;
        RedirectionTableEntry::PinPolarity polarity;
        RedirectionTableEntry::TriggerMode trigger;

        bool operator!=(const IrqOverride &other) const;
    };

    /**
     * MMIO accessible registers.
     *
     * ICH5 datasheet, sec. 9.5
     */
    enum Register : uint8_t {
        IND = 0x00, ///< Indirection register, for indirect register access
        DAT = 0x10, ///< Data register, for indirect register access
        IRQPA = 0x20,
        EOI = 0x40 ///< Dedicated end-of-interrupt register, since version 0x20
    };

    /**
     * Indirectly accessible registers.
     *
     * ICH5 datasheet, sec. 9.5
     */
    enum IndirectRegister : uint8_t {
        ID = 0x00,    ///< I/O APIC id.
        VER = 0x01,   ///< I/O APIC version.
        ARB = 0x02,   ///< Arbitration ID.
        REDTBL = 0x10 ///< Redirection table base address (24x 64 bit entry)
    };

    /**
     * Constructor.
     *
     * @param ioApicPlatform General information about the I/O APICs, parsed from ACPI
     * @param ioApicInformation Information about the specific I/O APIC, parsed from ACPI
     */
    IoApic(uint8_t ioId, void *baseAddress, Kernel::GlobalSystemInterrupt gsiBase);

    /**
     * Copy Constructor.
     */
    IoApic(const IoApic &other) = delete;

    /**
     * Assignment operator.
     */
    IoApic &operator=(const IoApic &other) = delete;

    /**
     * Destructor.
     */
    ~IoApic() = default;

    /**
     * Determine the I/O APIC version
     */
    uint8_t getVersion();

    /**
     * Initialize the I/O APIC.
     */
    void initialize();

    /**
     * Unmask an interrupt in this IO APIC.
     *
     * @param gsi The GSI to unmask
     */
    void allow(Kernel::GlobalSystemInterrupt gsi);

    /**
     * Mask an interrupt in the local APIC.
     *
     * @param gsi The GSI to mask
     */
    void forbid(Kernel::GlobalSystemInterrupt gsi);

    /**
     * Get the state of this interrupt - whether it is masked out or not.
     *
     * @param gsi The GSI
     * @return True, if the GSI is masked
     */
    bool status(Kernel::GlobalSystemInterrupt gsi);

    /**
     * Initialize a this I/O APIC's interrupt redirection table.
     *
     * Marks entries for all supported interrupt inputs of the IO APIC as edge-triggered, active high,
     * masked, physical destination mode to local APIC of the current CPU and fixed delivery mode,
     * unless handlePageFault mode or pin polarity are overridden. Sets vector numbers to corresponding InterruptVector.
     */
    void initializeRedirectionTable();

    /**
     * Check if an interrupt is non-maskable.
     */
    bool isNonMaskableInterrupt(Kernel::GlobalSystemInterrupt interrupt);

    /**
     * Configure an NMI for this I/O APIC, if one exists.
     */
    void addNonMaskableInterrupt(Kernel::GlobalSystemInterrupt nmiGsi, RedirectionTableEntry::PinPolarity nmiPolarity, RedirectionTableEntry::TriggerMode nmiTrigger);

    /**
     * Register a new IRQ override.
     *
     * @param source The PC/AT compatible IRQ
     * @param target The GSI that this system uses instead of the PC/AT compatible IRQ
     */
    void addIrqOverride(InterruptRequest source, Kernel::GlobalSystemInterrupt target, RedirectionTableEntry::PinPolarity polarity, RedirectionTableEntry::TriggerMode trigger);

    bool hasOverride(Kernel::GlobalSystemInterrupt target);

    bool hasOverride(InterruptRequest source);

    /**
     * Retrieve an IRQ override by supplying the target GSI.
     */
    const IrqOverride& getOverride(Kernel::GlobalSystemInterrupt target);

    /**
     * Retrieve an IRQ override by supplying the source IRQ.
     */
    const IrqOverride& getOverride(InterruptRequest source);

    /**
     * Read a MMIO register, identified by the offset to the I/O APIC base address.
     *
     * @tparam T The type of the register data, e.g. uint32_t
     */
    template<typename T>
    [[nodiscard]] T readMMIORegister(Register reg);

    /**
     * Write a MMIO register, identified by the offset to the I/O APIC base address.
     *
     * @tparam T The type of the register data, e.g. uint32_t
     */
    template<typename T>
    void writeMMIORegister(Register reg, T val);

    /**
     * Read an indirect register, identified by the address written to the indirection register.
     */
    [[nodiscard]] uint32_t readIndirectRegister(IndirectRegister reg);

    /**
     * Write an indirect register, identified by the address written to the indirection register.
     */
    void writeIndirectRegister(IndirectRegister reg, uint32_t val);

    /**
     * Write a redirection table entry, identified by the corresponding GSI.
     */
    [[nodiscard]] RedirectionTableEntry readRedirectionTableEntry(Kernel::GlobalSystemInterrupt gsi);

    /**
     * Read a redirection table entry, identified by the corresponding GSI.
     */
    void writeRedirectionTableEntry(Kernel::GlobalSystemInterrupt gsi, const RedirectionTableEntry &redtbl);

    Kernel::GlobalSystemInterrupt getMaxGlobalSystemInterruptNumber();

private:

    uint8_t ioId;
    void *mmioAddress = 0;

    Kernel::GlobalSystemInterrupt gsiBase{};
    Kernel::GlobalSystemInterrupt gsiMax{};
    Util::ArrayList<NmiSource> nmiSources;

    Util::ArrayList<IrqOverride> irqOverrides;
    Kernel::GlobalSystemInterrupt systemGsiMax{};

    Util::Async::Spinlock registerLock;
    Util::Async::Spinlock redirectionTableLock;
};

template<typename T>
T IoApic::readMMIORegister(Register reg) {
    return *reinterpret_cast<volatile T*>(static_cast<uint8_t*>(mmioAddress) + reg);
}

template<typename T>
void IoApic::writeMMIORegister(Register reg, T val) {
    *reinterpret_cast<volatile T*>(static_cast<uint8_t*>(mmioAddress) + reg) = val;
}

}

#endif
