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

#include "Apic.h"
#include "device/power/acpi/Acpi.h"

namespace Device {

Kernel::Logger Apic::log = Kernel::Logger::get("APIC");

Apic::Apic(const Util::Array<LocalApic*> &localApics, IoApic *ioApic) : localApics(localApics), localTimers(localApics.length()), ioApic(ioApic) {
    for (auto *&localTimer : localTimers) {
        localTimer = nullptr;
    }
}

bool Apic::isAvailable() {
    return LocalApic::supportsXApic() && Acpi::isAvailable() && Acpi::hasTable("APIC");
}

Apic* Apic::initialize() {
    if (!LocalApic::readBaseModelSpecificRegister().isBootstrapProcessor) {
        // IA32_APIC_BASE_MSR is unique (every core has its own)
        log.error("Apic may only be initialized by the bootstrap processor!");
        return nullptr;
    }

    auto localApics = getLocalApics();
    if (localApics.length() == 0) {
        return nullptr;
    }

    auto *ioApic = getIoApic();
    if (ioApic == nullptr) {
        return nullptr;
    }

    auto *apic = new Apic(localApics, ioApic);

    // Initialize our local APIC, all others are only initialized when SMP is started up
    const auto &madt = Acpi::getTable<Acpi::Madt>("APIC");
    log.info("Enabling xAPIC mode");
    LocalApic::enableXApicMode(madt.localApicAddress);
    log.info("Initializing local APIC [%u]", apic->getCurrentLocalApic().getCpuId());
    apic->initializeCurrentLocalApic();

    // Initialize the I/O APIC
    log.info("Initializing IO APIC");
    ioApic->initialize();

    // We only require one error handler, as every AP can only access its own local APIC's error register
    apic->errorHandler.plugin();
    enableCurrentErrorHandler();

    ApicTimer::calibrate();
    apic->startCurrentTimer();

    return apic;
}

void Apic::initializeCurrentLocalApic() {
    LocalApic &localApic = getCurrentLocalApic();

    if (localApic.getCpuId() != LocalApic::getId()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Apic: AP can only enable itself!");
    }

    localApic.initialize();
}

LocalApic& Apic::getCurrentLocalApic() {
    return *localApics[LocalApic::getId()];
}

void Apic::enableCurrentErrorHandler() {
    // This part needs to be done for each AP
    LocalApic::allow(LocalApic::ERROR);
    LocalApic::clearErrors(); // Arm the Error interrupt
}

void Apic::allow(InterruptRequest interruptRequest) {
    auto gsi = getIrqOverride(interruptRequest);
    if (ioApic->isNonMaskableInterrupt(gsi)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Apic: GSI is non maskable!");
    }

    ioApic->allow(gsi);
}

void Apic::forbid(InterruptRequest interruptRequest) {
    auto gsi = getIrqOverride(interruptRequest);
    if (ioApic->isNonMaskableInterrupt(gsi)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Apic: GSI is non maskable!");
    }

    ioApic->forbid(gsi);
}

bool Apic::status(InterruptRequest interruptRequest) {
    auto gsi = getIrqOverride(interruptRequest);
    return ioApic->status(gsi);
}

void Apic::sendEndOfInterrupt(Kernel::InterruptVector vector) {
    if (isLocalInterrupt(vector) && vector != Kernel::InterruptVector::LINT1) {
        // Excludes NMI, IPIs and SMIs are also excluded, but these don't have vector numbers,
        // so they won't reach this anyway.
        LocalApic::sendEndOfInterrupt();
    } else if (isExternalInterrupt(vector)) {
        // Edge-triggered external interrupts have to be EOId in the local APIC,
        // level-triggered external interrupts can EOId in the local APIC if EOI-broadcasting is enabled,
        // otherwise they can be directly EOId in the I/O APIC by using the I/O APICs EOI register or
        // masking them and setting them as edge-triggered temporarily (which clears the remote IRR bit).
        // Here, EOI broadcasting is enabled, which makes it very simple:
        LocalApic::sendEndOfInterrupt(); // External interrupts get forwarded by the local APIC, so local EOI required
    }
}

bool Apic::isLocalInterrupt(Kernel::InterruptVector vector) {
    return vector >= Kernel::InterruptVector::CMCI && vector <= Kernel::InterruptVector::ERROR;
}

bool Apic::isExternalInterrupt(Kernel::InterruptVector vector) {
    // Remapping can be ignored here, as all GSIs are contiguous anyway
    return static_cast<Kernel::GlobalSystemInterrupt>(vector - 32) <= ioApic->getMaxGlobalSystemInterruptNumber();
}

Util::Array<LocalApic*> Apic::getLocalApics() {
    auto localApics = Util::ArrayList<LocalApic*>();
    auto acpiLocalApics = Acpi::getMadtStructures<Acpi::ProcessorLocalApic>(Acpi::PROCESSOR_LOCAL_APIC);
    auto acpiLocalApicNmis = Acpi::getMadtStructures<Acpi::LocalApicNmi>(Acpi::LOCAL_APIC_NMI);

    if (acpiLocalApics.length() == 0) {
        log.error("No local APIC detected");
        return Util::Array<LocalApic*>(0);
    }

    log.info("[%u] local %s detected", acpiLocalApics.length(), acpiLocalApics.length() == 1 ? "APIC" : "APICs");

    for (const auto *localInfo : acpiLocalApics) {
        if (!(localInfo->flags & 0x01)) {
            // When ACPI reports this local APIC as disabled, it may not be used by the OS.
            // ACPI 1.0 specification, sec. 5.2.8.1
            log.info("Local APIC [%u] is marked as disabled", localInfo->apicId);
            localApics.add(localInfo->apicId, nullptr);
            continue;
        }

        // Find the NMI belonging to the current localInfo, every local APIC should have exactly one
        const Acpi::LocalApicNmi *nmiInfo = nullptr;
        for (const auto *localNmi : acpiLocalApicNmis) {
            // 0xff means all APs
            if ((localNmi->acpiProcessorId == localInfo->acpiProcessorId) | (localNmi->acpiProcessorId == 0xff)) {
                nmiInfo = localNmi;
                break;
            }
        }

        if (nmiInfo == nullptr) {
            log.error("Couldn't find NMI info for local APIC [%u]!", localInfo->apicId);
            return Util::Array<LocalApic*>(0);
        }

        auto *localApic = new LocalApic(localInfo->apicId, nmiInfo->localApicLint == 0 ? LocalApic::LINT0 : LocalApic::LINT1,
                nmiInfo->flags & Acpi::IntiFlag::ACTIVE_HIGH ? LocalApic::LocalVectorTableEntry::PinPolarity::HIGH : LocalApic::LocalVectorTableEntry::PinPolarity::LOW,
                nmiInfo->flags & Acpi::IntiFlag::EDGE_TRIGGERED ? LocalApic::LocalVectorTableEntry::TriggerMode::EDGE : LocalApic::LocalVectorTableEntry::TriggerMode::LEVEL);
        localApics.add(localInfo->apicId, localApic);
    }

    return localApics.toArray();
}

IoApic *Apic::getIoApic() {
    auto acpiIoApics = Acpi::getMadtStructures<Acpi::IoApic>(Acpi::IO_APIC);
    auto acpiNmiSources = Acpi::getMadtStructures<Acpi::NmiSource>(Acpi::NON_MASKABLE_INTERRUPT_SOURCE);
    auto acpiInterruptSourceOverrides = Acpi::getMadtStructures<Acpi::InterruptSourceOverride>(Acpi::INTERRUPT_SOURCE_OVERRIDE);

    if (acpiIoApics.length() == 0) {
        // This is illegal, because this implementation does not support virtual wire mode
        log.error("No IO APIC present");
        return nullptr;
    }

    // Multiple I/O APICs are possible, but in the usual Intel consumer chipsets there is only one
    if (acpiIoApics.length() > 1) {
        log.error("[%u] IO %s detected, but more than one IO APIC is not supported", acpiIoApics.length(), acpiIoApics.length() == 1 ? "APIC" : "APICs");
        return nullptr;
    }

    log.info("[%u] IO %s detected", acpiIoApics.length(), acpiIoApics.length() == 1 ? "APIC" : "APICs");
    log.info("[%u] interrupt source overrides found", acpiInterruptSourceOverrides.length());

    const auto *ioInfo = acpiIoApics[0];
    auto *ioApic = new IoApic(ioInfo->ioApicId, ioInfo->ioApicAddress, static_cast<Kernel::GlobalSystemInterrupt>(ioInfo->globalSystemInterruptBase));

    // Add all NMIs that belong to this I/O APIC
    for (const auto *nmi : acpiNmiSources) {
        ioApic->addNonMaskableInterrupt(static_cast<Kernel::GlobalSystemInterrupt>(nmi->globalSystemInterrupt),
                nmi->flags & Acpi::IntiFlag::ACTIVE_HIGH ? IoApic::RedirectionTableEntry::PinPolarity::HIGH : IoApic::RedirectionTableEntry::PinPolarity::LOW,
                nmi->flags & Acpi::IntiFlag::EDGE_TRIGGERED ? IoApic::RedirectionTableEntry::TriggerMode::EDGE : IoApic::RedirectionTableEntry::TriggerMode::LEVEL);
    }

    // Add the IRQ overrides
    for (const auto *override : acpiInterruptSourceOverrides) {
        // ISA bus default values
        auto polarity = IoApic::RedirectionTableEntry::PinPolarity::HIGH;
        auto trigger = IoApic::RedirectionTableEntry::TriggerMode::EDGE;

        // If flags[0:1] is 0, the bus default is used
        if ((override->flags & 0x3) != 0 && (override->flags & Acpi::IntiFlag::ACTIVE_LOW)) {
            // Use override instead of bus default (HIGH)
            polarity = IoApic::RedirectionTableEntry::PinPolarity::LOW;
        }

        // If flags[2:3] is 0, the bus default is used
        if ((override->flags & 0xc) != 0 && (override->flags & Acpi::IntiFlag::LEVEL_TRIGGERED)) {
            // Use override instead of bus default (EDGE)
            trigger = IoApic::RedirectionTableEntry::TriggerMode::LEVEL;
        }

        log.info("Adding interrupt source override ([%u] -> [%u])", override->source, override->globalSystemInterrupt);
        ioApic->addIrqOverride(static_cast<InterruptRequest>(override->source), static_cast<Kernel::GlobalSystemInterrupt>(override->globalSystemInterrupt), polarity, trigger);
    }

    return ioApic;
}

Kernel::GlobalSystemInterrupt Apic::getIrqOverride(InterruptRequest interruptRequest) {
    return ioApic->hasOverride(interruptRequest) ? ioApic->getOverride(interruptRequest).target : static_cast<Kernel::GlobalSystemInterrupt>(interruptRequest);
}

bool Apic::isCurrentTimerRunning() {
    return localTimers[LocalApic::getId()] != nullptr;
}

void Apic::startCurrentTimer() {
    if (isCurrentTimerRunning()) {
        log.warn("Trying to start an already running APIC timer");
        return;
    }

    auto *apicTimer = new Device::ApicTimer();
    apicTimer->plugin();
    localTimers[LocalApic::getId()] = apicTimer;
}

ApicTimer& Apic::getCurrentTimer() {
    if (!isCurrentTimerRunning()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Timer for the current CPU has not been initialized yet!");
    }

    return *localTimers[LocalApic::getId()];
}

}
