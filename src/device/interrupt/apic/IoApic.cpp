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

#include "IoApic.h"

#include "kernel/system/System.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/base/Constants.h"
#include "LocalApic.h"
#include "lib/util/base/Exception.h"
#include "lib/util/collection/Iterator.h"

namespace Kernel {
enum GlobalSystemInterrupt : uint32_t;
enum InterruptVector : uint8_t;
}  // namespace Kernel

namespace Device {
enum InterruptRequest : uint8_t;

IoApic::IoApic(uint8_t ioId, uint32_t baseAddress, Kernel::GlobalSystemInterrupt gsiBase) : ioId(ioId), gsiBase(gsiBase) {
    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    void *virtualStartAddress = memoryService.mapIO(baseAddress, Util::PAGESIZE, true);

    // Account for possible misalignment
    const uint32_t pageOffset = baseAddress % Util::PAGESIZE;
    mmioAddress = reinterpret_cast<uint32_t>(virtualStartAddress) + pageOffset;
}

uint8_t IoApic::getVersion() {
    return readIndirectRegister(VER) & 0xFF;
}

void IoApic::initialize() {
    // Set the I/O APIC ID (the id register is initialized to 0) read from ACPI
    writeIndirectRegister(ID, static_cast<uint32_t>(ioId) << 24); // ICH5, sec. 9.5.6

    // With the IRQPA there is a way to address more than 255 GSIs although maxREDTBLEntries only has 8 bits
    // With ICH5 and other ICHs it is always 24 (ICH5 only has 1 IO APIC, as other consumer hardware)
    gsiMax = static_cast<Kernel::GlobalSystemInterrupt>(gsiBase + (readIndirectRegister(VER) >> 16));
    if (gsiMax > systemGsiMax) {
        systemGsiMax = gsiMax;
    }

    initializeRedirectionTable();

    // Configure NMIs
    for (const auto &nmi : nmiSources) {
        RedirectionTableEntry entry{};
        entry.vector = static_cast<Kernel::InterruptVector>(0);
        entry.deliveryMode = RedirectionTableEntry::DeliveryMode::NMI;
        entry.destinationMode = RedirectionTableEntry::DestinationMode::PHYSICAL;
        entry.pinPolarity = nmi.polarity;
        entry.triggerMode = nmi.trigger;
        entry.isMasked = false;
        entry.destination = LocalApic::getId(); // Send to the bootstrap processor
        writeRedirectionTableEntry(nmi.source, entry);
    }
}

void IoApic::allow(Kernel::GlobalSystemInterrupt gsi) {
    auto entry = readRedirectionTableEntry(gsi);
    entry.isMasked = false;
    writeRedirectionTableEntry(gsi, entry);
}

void IoApic::forbid(Kernel::GlobalSystemInterrupt gsi) {
    auto entry = readRedirectionTableEntry(gsi);
    entry.isMasked = true;
    writeRedirectionTableEntry(gsi, entry);
}

bool IoApic::status(Kernel::GlobalSystemInterrupt gsi) {
    auto entry = readRedirectionTableEntry(gsi);
    return entry.isMasked;
}

void IoApic::initializeRedirectionTable() {
    RedirectionTableEntry entry{};
    entry.deliveryMode = RedirectionTableEntry::DeliveryMode::FIXED;
    entry.destinationMode = RedirectionTableEntry::DestinationMode::PHYSICAL;
    entry.isMasked = true;
    entry.destination = LocalApic::getId(); // All interrupts are sent to the bootstrap processor, which can be inefficient!

    for (uint32_t i = gsiBase; i <= gsiMax; ++i) {
        auto gsi = static_cast<Kernel::GlobalSystemInterrupt>(i); // GSIs match interrupt inputs on IO APIC

        entry.vector = static_cast<Kernel::InterruptVector>(gsi + 32); // If no override exists GSI matches vector
        entry.pinPolarity = RedirectionTableEntry::PinPolarity::HIGH;  // ISA bus default
        entry.triggerMode = RedirectionTableEntry::TriggerMode::EDGE;  // ISA bus default

        if (hasOverride(gsi)) {
            const auto &override = getOverride(gsi);
            // Apply a mapping differing from the identity mapping
            entry.vector = static_cast<Kernel::InterruptVector>(override.source + 32);
            entry.pinPolarity = override.polarity;
            entry.triggerMode = override.trigger;
        }

        writeRedirectionTableEntry(gsi, entry);
    }
}

bool IoApic::isNonMaskableInterrupt(Kernel::GlobalSystemInterrupt interrupt) {
    for (uint32_t i = 0; i < nmiSources.size(); ++i) {
        const NmiSource &nmi = nmiSources.get(i);
        if (nmi.source == interrupt) {
            return true;
        }
    }

    return false;
}

void IoApic::addNonMaskableInterrupt(Kernel::GlobalSystemInterrupt nmiGsi, IoApic::RedirectionTableEntry::PinPolarity nmiPolarity, IoApic::RedirectionTableEntry::TriggerMode nmiTrigger) {
    nmiSources.add({nmiGsi, nmiPolarity, nmiTrigger});
}

void IoApic::addIrqOverride(InterruptRequest source, Kernel::GlobalSystemInterrupt target, IoApic::RedirectionTableEntry::PinPolarity polarity, IoApic::RedirectionTableEntry::TriggerMode trigger) {
    irqOverrides.add({source, target, polarity, trigger});
}

bool IoApic::hasOverride(Kernel::GlobalSystemInterrupt target) {
    for (const auto &override : irqOverrides) {
        if (override.target == target) {
            return true;
        }
    }

    return false;
}

bool IoApic::hasOverride(InterruptRequest source) {
    for (const auto &override : irqOverrides) {
        if (override.source == source) {
            return true;
        }
    }

    return false;
}

const IoApic::IrqOverride& IoApic::getOverride(Kernel::GlobalSystemInterrupt target) {
    for (const auto &override : irqOverrides) {
        if (override.target == target) {
            return override;
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "IoApic: No override found for given target!");
}

const IoApic::IrqOverride& IoApic::getOverride(InterruptRequest source) {
    for (const auto &override : irqOverrides) {
        if (override.source == source) {
            return override;
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "IoApic: No override found for given source!");
}

uint32_t IoApic::readIndirectRegister(IoApic::IndirectRegister reg) {
    registerLock.acquire(); // This needs to be synchronized in case multiple APs access a MMIO register
    writeMMIORegister<uint8_t>(IND, reg);
    auto val = readMMIORegister<uint32_t>(DAT);
    registerLock.release();
    return val;
}

void IoApic::writeIndirectRegister(IoApic::IndirectRegister reg, uint32_t val) {
    registerLock.acquire(); // This needs to be synchronized in case multiple APs access a MMIO register
    writeMMIORegister<uint8_t>(IND, reg);
    writeMMIORegister<uint32_t>(DAT, val);
    registerLock.release();
}

IoApic::RedirectionTableEntry IoApic::readRedirectionTableEntry(Kernel::GlobalSystemInterrupt gsi) {
    // The first register is the low DW, the second register is the high DW
    redirectionTableLock.acquire(); // This needs to be synchronized in case multiple APs access the REDTBL
    const uint32_t low = readIndirectRegister(static_cast<IndirectRegister>(REDTBL + 2 * gsi));
    const uint64_t high = readIndirectRegister(static_cast<IndirectRegister>(REDTBL + 2 * gsi + 1));
    redirectionTableLock.release();
    return static_cast<RedirectionTableEntry>(low | high << 32);
}

void IoApic::writeRedirectionTableEntry(Kernel::GlobalSystemInterrupt gsi, const IoApic::RedirectionTableEntry &redtbl) {
    // The first register is the low DW, the second register is the high DW
    auto val = static_cast<uint64_t>(redtbl);
    redirectionTableLock.acquire(); // This needs to be synchronized in case multiple APs access the REDTBL
    writeIndirectRegister(static_cast<IndirectRegister>(REDTBL + 2 * gsi), val & 0xffffffff);
    writeIndirectRegister(static_cast<IndirectRegister>(REDTBL + 2 * gsi + 1), val >> 32);
    redirectionTableLock.release();
}

Kernel::GlobalSystemInterrupt IoApic::getMaxGlobalSystemInterruptNumber() {
    return gsiMax;
}

IoApic::RedirectionTableEntry::RedirectionTableEntry(uint64_t registerValue) :
    vector(static_cast<Kernel::InterruptVector>(registerValue & 0xFF)),
    deliveryMode(static_cast<DeliveryMode>((registerValue & (0b111 << 8)) >> 8)),
    destinationMode(static_cast<DestinationMode>((registerValue & (1 << 11)) >> 11)),
    deliveryStatus(static_cast<DeliveryStatus>((registerValue & (1 << 12)) >> 12)),
    pinPolarity(static_cast<PinPolarity>((registerValue & (1 << 13)) >> 13)),
    triggerMode(static_cast<TriggerMode>((registerValue & (1 << 15)) >> 15)),
    isMasked(registerValue & (1 << 16)),
    destination(registerValue >> 56) {}

IoApic::RedirectionTableEntry::operator uint64_t() const {
    return static_cast<uint64_t>(vector) | static_cast<uint64_t>(deliveryMode) << 8 | static_cast<uint64_t>(destinationMode) << 11 | static_cast<uint64_t>(pinPolarity) << 13 | static_cast<uint64_t>(triggerMode) << 15 | static_cast<uint64_t>(isMasked) << 16 | static_cast<uint64_t>(destination) << 56;
}

bool IoApic::NmiSource::operator!=(const IoApic::NmiSource &other) const {
    return source != other.source || trigger != other.trigger || polarity != other.polarity;
}

bool IoApic::IrqOverride::operator!=(const IoApic::IrqOverride &other) const {
    return source != other.source || target != other.target || trigger != other.trigger || polarity != other.polarity;
}

}