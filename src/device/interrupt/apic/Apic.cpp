/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "Apic.h"

#include "device/system/Acpi.h"
#include "device/time/rtc/Cmos.h"
#include "device/cpu/SymmetricMultiprocessing.h"
#include "kernel/interrupt/InterruptVector.h"
#include "lib/util/base/Constants.h"
#include "kernel/memory/GlobalDescriptorTable.h"
#include "device/interrupt/apic/IoApic.h"
#include "device/interrupt/apic/LocalApic.h"
#include "device/interrupt/apic/LocalApicErrorHandler.h"
#include "kernel/log/Log.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Panic.h"
#include "lib/util/collection/ArrayList.h"
#include "kernel/memory/MemoryLayout.h"
#include "lib/util/hardware/Acpi.h"
#include "kernel/service/InformationService.h"
#include "device/cpu/Cpu.h"
#include "kernel/service/Service.h"
#include "kernel/service/TimeService.h"
#include "device/time/apic/ApicTimer.h"
#include "lib/util/time/Timestamp.h"

namespace Kernel {
enum GlobalSystemInterrupt : uint32_t;
}  // namespace Kernel

namespace Device {
enum InterruptRequest : uint8_t;

uint8_t initializedApplicationProcessorsCounter = 0; // Used to determine AP GDT/Stack slot

Apic::Apic(const Util::Array<LocalApic*> &localApics, IoApic *ioApic) : localApics(localApics), ioApic(ioApic) {}

bool Apic::isAvailable() {
    const auto &acpi = Kernel::Service::getService<Kernel::InformationService>().getAcpi();
    return LocalApic::supportsXApic() && acpi.getTables().hasTable("APIC");
}

Apic* Apic::initialize() {
    if (!LocalApic::readBaseModelSpecificRegister().isBootstrapProcessor) {
        // IA32_APIC_BASE_MSR is unique (every core has its own)
        LOG_ERROR("Apic may only be initialized by the bootstrap processor!");
        return nullptr;
    }

    auto localApics = initializeLocalApics();
    if (localApics.length() == 0) {
        return nullptr;
    }

    auto *ioApic = initializeIoApic();
    if (ioApic == nullptr) {
        return nullptr;
    }

    auto *apic = new Apic(localApics, ioApic);

    // Initialize our local APIC, all others are only initialized when SMP is started up
    const auto &acpi = Kernel::Service::getService<Kernel::InformationService>().getAcpi();
    const auto &madt = reinterpret_cast<const Util::Hardware::Acpi::Madt&>(acpi.getTables()["APIC"]);
    LOG_INFO("Enabling xAPIC mode");
    LocalApic::enableXApicMode(reinterpret_cast<void *>(madt.localApicAddress));
    LOG_INFO("Initializing local APIC [%u]", apic->getCurrentLocalApic().getCpuId());
    apic->initializeCurrentLocalApic();

    // Initialize the I/O APIC
    LOG_INFO("Initializing IO APIC");
    ioApic->initialize();

    // We only require one error handler, as every AP can only access its own local APIC's error register
    apic->errorHandler.plugin();
    apic->enableCurrentErrorHandler();

    return apic;
}

void Apic::initializeCurrentLocalApic() {
    getCurrentLocalApic().initialize();
}

LocalApic& Apic::getCurrentLocalApic() {
    for (auto *localApic : localApics) {
        if (localApic->getCpuId() == LocalApic::getId()) {
            return *localApic;
        }
    }

    Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Apic: Local APIC not found!");
}

void Apic::enableCurrentErrorHandler() {
    // This part needs to be done for each AP
    LocalApic::allow(LocalApic::ERROR);
    LocalApic::clearErrors(); // Arm the Error interrupt
}

void Apic::allow(InterruptRequest interruptRequest) {
    auto gsi = getIrqOverride(interruptRequest);
    if (ioApic->isNonMaskableInterrupt(gsi)) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Apic: GSI is non maskable!");
    }

    ioApic->allow(gsi);
}

void Apic::forbid(InterruptRequest interruptRequest) {
    auto gsi = getIrqOverride(interruptRequest);
    if (ioApic->isNonMaskableInterrupt(gsi)) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Apic: GSI is non maskable!");
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

bool Apic::isLocalInterrupt(Kernel::InterruptVector vector) const {
    return vector >= Kernel::InterruptVector::CMCI && vector <= Kernel::InterruptVector::ERROR;
}

bool Apic::isExternalInterrupt(Kernel::InterruptVector vector) const {
    // Remapping can be ignored here, as all GSIs are contiguous anyway
    return static_cast<Kernel::GlobalSystemInterrupt>(vector - 32) <= ioApic->getMaxGlobalSystemInterruptNumber();
}

Util::Array<LocalApic*> Apic::initializeLocalApics() {
    const auto &acpi = Kernel::Service::getService<Kernel::InformationService>().getAcpi();
    const auto &madt = reinterpret_cast<const Util::Hardware::Acpi::Madt&>(acpi.getTables()["APIC"]);
    auto localApics = Util::ArrayList<LocalApic*>();
    auto acpiLocalApics = madt.findStructures<Util::Hardware::Acpi::ProcessorLocalApic>(Util::Hardware::Acpi::PROCESSOR_LOCAL_APIC);
    auto acpiLocalApicNmis = madt.findStructures<Util::Hardware::Acpi::LocalApicNmi>(Util::Hardware::Acpi::LOCAL_APIC_NMI);

    if (acpiLocalApics.length() == 0) {
        LOG_ERROR("No local APIC detected");
        return Util::Array<LocalApic*>(0);
    }

    LOG_INFO("[%u] local %s detected", acpiLocalApics.length(), acpiLocalApics.length() == 1 ? "APIC" : "APICs");

    for (const auto *localInfo : acpiLocalApics) {
        if (!(localInfo->flags & 0x01)) {
            // When ACPI reports this local APIC as disabled, it may not be used by the OS.
            // ACPI 1.0 specification, sec. 5.2.8.1
            LOG_INFO("Local APIC [%u] is marked as disabled", localInfo->apicId);
            continue;
        }

        auto *localApic = new LocalApic(localInfo->apicId);

        // Find the NMI belonging to the current localInfo
        // const Acpi::LocalApicNmi *nmiInfo = nullptr;
        // for (const auto *localNmi : acpiLocalApicNmis) {
        //     // 0xff means all APs
        //     if ((localNmi->acpiProcessorId == localInfo->acpiProcessorId) || (localNmi->acpiProcessorId == 0xff)) {
        //         nmiInfo = localNmi;
        //         break;
        //     }
        // }
        // if (nmiInfo == nullptr) {
        //     LOG_ERROR("Couldn't find NMI info for local APIC [%u]!", localInfo->apicId);
        //     return Util::Array<LocalApic*>(0);
        // }
        // localApic->addNonMaskableInterrupt(nmiInfo->localApicLint == 0 ? LocalApic::LINT0 : LocalApic::LINT1,
        //                                    nmiInfo->flags & Acpi::IntiFlag::ACTIVE_HIGH ? LocalApic::LocalVectorTableEntry::PinPolarity::HIGH : LocalApic::LocalVectorTableEntry::PinPolarity::LOW,
        //                                    nmiInfo->flags & Acpi::IntiFlag::EDGE_TRIGGERED ? LocalApic::LocalVectorTableEntry::TriggerMode::EDGE : LocalApic::LocalVectorTableEntry::TriggerMode::LEVEL);

        // The above way of assigning NMIs is too strict:
        // - Some firmwares (e.g. Lenovo T440s) report wrong values for the NMI's acpiProcessorId
        //   that don't match with the local APIC's acpiProcessorId, so they can't be assigned
        // - Not every core has to handle the NMI, the BSP suffices, done in Linux:
        //   https://github.com/torvalds/linux/commit/b7c4948e9881fb38b048269f376fb4bf194ce24a
        // - CPUs with hyperthreading report local APICs also for logical cores, but those don't
        //   get NMI definitions
        // It is also not necessary to configure the NMI depending on ACPI, as LINT1/edge-triggered
        // is specified in the MultiProcessor Specification (sec. 5.2) and IA-32 manual (3.11.5.1).
        // - I couldn't find information on the pin-polarity, but Linux uses active-high:
        //   https://github.com/torvalds/linux/blob/master/arch/x86/include/asm/apicdef.h#L86
        // The NMI implementation is kept very general still (similar to IoApic), because I can't
        // find definitive information on what configurations are theoretically valid
        if (localApic->getCpuId() == 0) {
            localApic->addNonMaskableInterrupt(LocalApic::LINT1,
                                               LocalApic::LocalVectorTableEntry::PinPolarity::HIGH,
                                               LocalApic::LocalVectorTableEntry::TriggerMode::EDGE);
        }

        localApics.add(localApic);
    }

    LOG_INFO("[%u] local %s usable", localApics.size(), localApics.size() == 1 ? "APIC is" : "APICs are");

    return localApics.toArray();
}

IoApic *Apic::initializeIoApic() {
    const auto &acpi = Kernel::Service::getService<Kernel::InformationService>().getAcpi();
    const auto &madt = reinterpret_cast<const Util::Hardware::Acpi::Madt&>(acpi.getTables()["APIC"]);
    auto acpiIoApics = madt.findStructures<Util::Hardware::Acpi::IoApic>(Util::Hardware::Acpi::IO_APIC);
    auto acpiNmiSources = madt.findStructures<Util::Hardware::Acpi::NmiSource>(Util::Hardware::Acpi::NON_MASKABLE_INTERRUPT_SOURCE);
    auto acpiInterruptSourceOverrides = madt.findStructures<Util::Hardware::Acpi::InterruptSourceOverride>(Util::Hardware::Acpi::INTERRUPT_SOURCE_OVERRIDE);

    if (acpiIoApics.length() == 0) {
        // This is illegal, because this implementation does not support virtual wire mode
        LOG_ERROR("No IO APIC present");
        return nullptr;
    }

    // Multiple I/O APICs are possible, but in the usual Intel consumer chipsets there is only one
    if (acpiIoApics.length() > 1) {
        LOG_ERROR("[%u] IO %s detected, but more than one IO APIC is not supported", acpiIoApics.length(), acpiIoApics.length() == 1 ? "APIC" : "APICs");
        return nullptr;
    }

    LOG_INFO("[%u] IO %s detected", acpiIoApics.length(), acpiIoApics.length() == 1 ? "APIC" : "APICs");
    LOG_INFO("[%u] interrupt source %s found", acpiInterruptSourceOverrides.length(), acpiInterruptSourceOverrides.length() == 1 ? "override" : "overrides");

    const auto *ioInfo = acpiIoApics[0];
    auto *ioApic = new IoApic(ioInfo->ioApicId, reinterpret_cast<void*>(ioInfo->ioApicAddress), static_cast<Kernel::GlobalSystemInterrupt>(ioInfo->globalSystemInterruptBase));

    // Add all NMIs that belong to this I/O APIC
    for (const auto *nmi : acpiNmiSources) {
        ioApic->addNonMaskableInterrupt(static_cast<Kernel::GlobalSystemInterrupt>(nmi->globalSystemInterrupt),
                nmi->flags & Util::Hardware::Acpi::IntiFlag::ACTIVE_HIGH ? IoApic::RedirectionTableEntry::PinPolarity::HIGH : IoApic::RedirectionTableEntry::PinPolarity::LOW,
                nmi->flags & Util::Hardware::Acpi::IntiFlag::EDGE_TRIGGERED ? IoApic::RedirectionTableEntry::TriggerMode::EDGE : IoApic::RedirectionTableEntry::TriggerMode::LEVEL);
    }

    // Add the IRQ overrides
    for (const auto *override : acpiInterruptSourceOverrides) {
        // ISA bus default values
        auto polarity = IoApic::RedirectionTableEntry::PinPolarity::HIGH;
        auto trigger = IoApic::RedirectionTableEntry::TriggerMode::EDGE;

        // If flags[0:1] is 0, the bus default is used
        if ((override->flags & 0x3) != 0 && (override->flags & Util::Hardware::Acpi::IntiFlag::ACTIVE_LOW)) {
            // Use override instead of bus default (HIGH)
            polarity = IoApic::RedirectionTableEntry::PinPolarity::LOW;
        }

        // If flags[2:3] is 0, the bus default is used
        if ((override->flags & 0xc) != 0 && (override->flags & Util::Hardware::Acpi::IntiFlag::LEVEL_TRIGGERED)) {
            // Use override instead of bus default (EDGE)
            trigger = IoApic::RedirectionTableEntry::TriggerMode::LEVEL;
        }

        LOG_INFO("Interrupt source override [%u]->[%u], Polarity: [%s], Trigger: [%s]", override->source, override->globalSystemInterrupt,
                 polarity == IoApic::RedirectionTableEntry::PinPolarity::HIGH ? "High" : "Low", trigger == IoApic::RedirectionTableEntry::TriggerMode::EDGE ? "Edge" : "Level");
        ioApic->addIrqOverride(static_cast<InterruptRequest>(override->source), static_cast<Kernel::GlobalSystemInterrupt>(override->globalSystemInterrupt), polarity, trigger);
    }

    return ioApic;
}

Kernel::GlobalSystemInterrupt Apic::getIrqOverride(InterruptRequest interruptRequest) {
    return ioApic->hasOverride(interruptRequest) ? ioApic->getOverride(interruptRequest).target : static_cast<Kernel::GlobalSystemInterrupt>(interruptRequest);
}

InterruptRequest Apic::getIrqSource(Kernel::GlobalSystemInterrupt gsi) {
    return ioApic->hasOverride(gsi) ? ioApic->getOverride(gsi).source : static_cast<InterruptRequest>(gsi);
}

Kernel::GlobalSystemInterrupt Apic::getMaxInterruptTarget() {
    return ioApic->getMaxGlobalSystemInterruptNumber();
}

void Apic::startCurrentTimer() {
    getCurrentLocalApic().startTimer();
}

ApicTimer& Apic::getCurrentTimer() {
    return getCurrentLocalApic().getTimer();
}

bool Apic::isSymmetricMultiprocessingSupported() const {
    return localApics.length() > 1;
}

uint8_t Apic::getCoreCount() const {
    return localApics.length();
}

void Apic::startupApplicationProcessors() {
    auto &timeService = Kernel::Service::getService<Kernel::TimeService>();
    auto *gdtPointers = prepareApplicationProcessorGdts();
    auto *stackPointers = prepareApplicationProcessorStacks();
    prepareApplicationProcessorStartupCode(gdtPointers, stackPointers);
    prepareApplicationProcessorWarmReset(); // This is technically only required for discrete APIC, see below

    // Universal Startup Algorithm requires all interrupts disabled (they should be disabled anyway, but disabling them a second time is twice as good)
    Cpu::disableInterrupts();
    Cmos::disableNmi();

    LOG_INFO("CPU [%u] is the bootstrap processor", LocalApic::getId());

    // Call the startup code on each AP using the INIT-SIPI-SIPI Universal Startup Algorithm
    for (const auto *localApic : localApics) {
        if (localApic->getCpuId() == LocalApic::getId()) {
            // Skip this AP if it's the BSP
            continue;
        }

        // Info on discrete APIC:
        // The INIT IPI is required for CPUs with a discrete APIC, these ignore the STARTUP IPI.
        // For these CPUs, the startup routines address has to be written to the BIOS memory segment
        // (warm reset vector), and the AP has to be configured for warm-reset to start executing there.
        // This is unused for xApic. The INIT IPI is still issued though, to follow the IA-32 manual's
        // "INIT-SIPI-SIPI" sequence and the "universal startup algorithm" (MPSpec, sec. B.4):
        LocalApic::clearErrors();
        // Level-triggered, needs to be...
        LocalApic::sendInitInterProcessorInterrupt(localApic->getCpuId(), LocalApic::InterruptCommandRegisterEntry::Level::ASSERT);
        // xv6 waits 200 us instead.
        LocalApic::waitForInterProcessorInterruptDispatch();
        // ...deasserted manually
        LocalApic::sendInitInterProcessorInterrupt(localApic->getCpuId(), LocalApic::InterruptCommandRegisterEntry::Level::DEASSERT);
        // Not necessary with 10ms delay
        LocalApic::waitForInterProcessorInterruptDispatch();
        // Wait 100 us.
        timeService.busyWait(Util::Time::Timestamp::ofMicroseconds(100));

        // Issue the SIPI twice (for xApic):
        for (uint8_t j = 0; j < 2; ++j) {
            LocalApic::clearErrors();
            LocalApic::sendStartupInterProcessorInterrupt(localApic->getCpuId(), Kernel::MemoryLayout::APPLICATION_PROCESSOR_STARTUP_CODE.startAddress);
            LocalApic::waitForInterProcessorInterruptDispatch();
            timeService.busyWait(Util::Time::Timestamp::ofMicroseconds(200));
        }

        // Wait until the AP marks itself as running, so we can continue to the next one.
        // Because we initialize the APs one at a time, runningAPs is not synchronized.
        // If the AP initialization fails (and the system doesn't crash), this will lock the BSP,
        // the same will happen if the SIPI does not reach its target. That's why we abort.
        // Because the system time is not yet functional, we delay to measure the time.
        uint32_t readCount = 0;
        while (!runningApplicationProcessors[initializedApplicationProcessorsCounter]) {
            if (readCount > 10) {
                // Waited 10 * 10 ms = 0.1 s in total (pretty arbitrarily chosen by me)
                LOG_ERROR("CPU [%u] did not mark itself as running, it could be in undefined state", localApic->getCpuId());
                break;
            }

            timeService.busyWait(Util::Time::Timestamp::ofMilliseconds(10));
            readCount++;
        }

        initializedApplicationProcessorsCounter++;
        LOG_INFO("CPU [%u] is now online", localApic->getCpuId());
    }

    Cmos::enableNmi();
    Cpu::enableInterrupts();

    // Free the stack pointer array and the gdt now that all APs are running.
    // Keep the stacks and gdts though, they are not temporary!
    delete[] reinterpret_cast<uint8_t*>(gdtPointers);
    delete[] reinterpret_cast<uint8_t*>(stackPointers);
}

uint8_t** Apic::prepareApplicationProcessorStacks() {
    // Allocate the stack pointer array
    auto **stacks = new uint8_t*[localApics.length() - 1]; // Exclude BSP

    // Allocate the stacks
    for (uint32_t i = 0; i < localApics.length() - 1; ++i) {
        stacks[i] = new uint8_t[AP_STACK_SIZE];
    }

    return stacks;
}

void Apic::prepareApplicationProcessorStartupCode(void *gdts, void *stacks) {
    if (boot_ap_size > Util::PAGESIZE) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Startup code does not fit into one page!");
    }

    // Prepare the empty variables in the startup routine at their original location
    boot_ap_idt = Kernel::InterruptDescriptorTable::Descriptor::read();
    boot_ap_cr0 = Device::Cpu::readCr0();
    boot_ap_cr3 = reinterpret_cast<uint32_t>(Device::Cpu::readCr3());
    boot_ap_cr4 = Device::Cpu::readCr4();
    boot_ap_gdts = reinterpret_cast<uint32_t>(gdts);
    boot_ap_stacks = reinterpret_cast<uint32_t>(stacks);
    boot_ap_entry = reinterpret_cast<uint32_t>(&applicationProcessorEntry);

    // Copy the startup routine and prepared variables to the identity mapped page
    const auto startupCode = Util::Address(reinterpret_cast<uint32_t>(&boot_ap));
    const auto destination = Util::Address(Kernel::MemoryLayout::APPLICATION_PROCESSOR_STARTUP_CODE.startAddress);
    destination.copyRange(startupCode, boot_ap_size);
}

void Apic::prepareApplicationProcessorWarmReset() {
    Cmos::write(0xF, 0x0A); // Shutdown status byte (MPSpec, sec. B.4)

    auto *warmResetVector = reinterpret_cast<uint16_t*>(0x40 << 4 | 0x67); // MPSpec, sec. B.4

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds" // GCC complains about the reset vector being out of bounds, but it is not (it is just located at a very low address)
    *warmResetVector = Kernel::MemoryLayout::APPLICATION_PROCESSOR_STARTUP_CODE.startAddress;
#pragma GCC diagnostic pop
}

Kernel::GlobalDescriptorTable::Descriptor** Apic::prepareApplicationProcessorGdts() {
    // Allocate descriptor pointer array
    auto **gdts = new Kernel::GlobalDescriptorTable::Descriptor*[localApics.length() - 1]{}; // Skip BSP

    // Create GDTs for each core
    for (uint32_t i = 0; i < localApics.length() - 1; ++i) {
        auto *gdt = new Kernel::GlobalDescriptorTable{};
        auto *tss = new Kernel::GlobalDescriptorTable::TaskStateSegment{};

        gdt->addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x9a, 0x0c)); // Kernel code segment
        gdt->addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x92, 0x0c)); // Kernel data segment
        gdt->addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xfa, 0x0c)); // User code segment
        gdt->addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xf2, 0x0c)); // User data segment
        gdt->addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(reinterpret_cast<uint32_t>(&tss), sizeof(Kernel::GlobalDescriptorTable::TaskStateSegment), 0x89, 0x04));

        // Store current GDT descriptor in array
        gdts[i] = new Kernel::GlobalDescriptorTable::Descriptor(gdt->getDescriptor());
    }

    return gdts;
}

const Util::Array<LocalApic *> &Apic::getLocalApics() const {
    return localApics;
}

}
