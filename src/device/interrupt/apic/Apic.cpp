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
#include "device/time/Cmos.h"
#include "device/time/Pit.h"
#include "device/cpu/symmetric_multiprocessing.h"
#include "kernel/system/System.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/base/Constants.h"
#include "kernel/paging/Paging.h"
#include "kernel/system/TaskStateSegment.h"

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
    apic->enableCurrentErrorHandler();

    ApicTimer::calibrate();
    apic->startCurrentTimer();

    return apic;
}

void Apic::initializeCurrentLocalApic() {
    getCurrentLocalApic().initialize();
}

LocalApic& Apic::getCurrentLocalApic() {
    // Instead of indexing the local APIC by their IDs, search the full array.
    // This is required, as local APIC IDs might not be sequential or 0-based.
    const auto currentCpuId = LocalApic::getId();
    for (auto *localApic : localApics) {
        if (localApic->getCpuId() == currentCpuId) {
            return *localApic;
        }
    }

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Apic: No local APIC found for current CPU.");
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

bool Apic::isLocalInterrupt(Kernel::InterruptVector vector) const {
    return vector >= Kernel::InterruptVector::CMCI && vector <= Kernel::InterruptVector::ERROR;
}

bool Apic::isExternalInterrupt(Kernel::InterruptVector vector) const {
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
        localApics.add(localApic);
    }

    log.info("[%u] local %s are usable", localApics.size(), localApics.size() == 1 ? "APIC" : "APICs");

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

        log.info("Interrupt source override [%u]->[%u], Polarity: [%s], Trigger: [%s]", override->source, override->globalSystemInterrupt,
                 polarity == IoApic::RedirectionTableEntry::PinPolarity::HIGH ? "High" : "Low", trigger == IoApic::RedirectionTableEntry::TriggerMode::EDGE ? "Edge" : "Level");
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

    auto *apicTimer = new Device::ApicTimer(10, 10);
    apicTimer->plugin();
    localTimers[LocalApic::getId()] = apicTimer;
}

ApicTimer& Apic::getCurrentTimer() {
    if (!isCurrentTimerRunning()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Timer for the current CPU has not been initialized yet!");
    }

    return *localTimers[LocalApic::getId()];
}

bool Apic::isSymmetricMultiprocessingSupported() const {
    return localApics.length() > 1;
}

void Apic::startupApplicationProcessors() {
    void *gdtPointers = prepareApplicationProcessorGdts();
    void *stackPointers = prepareApplicationProcessorStacks();
    void *startupCodeRegion = prepareApplicationProcessorStartupCode(gdtPointers, stackPointers);
    void *warmResetVectorRegion = prepareApplicationProcessorWarmReset(); // This is technically only required for discrete APIC, see below

    // Universal Startup Algorithm requires all interrupts disabled (they should be disabled anyway, but disabling them a second time is twice as good)
    Cpu::disableInterrupts();
    Cmos::disableNmi();

    // Call the startup code on each AP using the INIT-SIPI-SIPI Universal Startup Algorithm
    for (uint32_t i = 0; i < localApics.length(); ++i) {
        const LocalApic *localApic = localApics[i];
        if (localApic == nullptr || localApic->getCpuId() == LocalApic::getId()) {
            // Skip this AP if it's the BSP or disabled
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
        // 10 ms, xv6 waits 100 us instead.
        Pit::earlyDelay(10000);

        // Issue the SIPI twice (for xApic):
        for (uint8_t j = 0; j < 2; ++j) {
            LocalApic::clearErrors();
            LocalApic::sendStartupInterProcessorInterrupt(localApic->getCpuId(), applicationProcessorStartupAddress);
            LocalApic::waitForInterProcessorInterruptDispatch();
            Pit::earlyDelay(200); // 200 us
        }

        // Wait until the AP marks itself as running, so we can continue to the next one.
        // Because we initialize the APs one at a time, runningAPs is not synchronized.
        // If the AP initialization fails (and the system doesn't crash), this will lock the BSP,
        // the same will happen if the SIPI does not reach its target. That's why we abort.
        // Because the system time is not yet functional, we delay to measure the time.
        uint32_t readCount = 0;
        while (!runningApplicationProcessors[localApic->getCpuId()]) {
            if (readCount > 10) {
                // Waited 10 * 10 ms = 0.1 s in total (pretty arbitrarily chosen by me)
                log.error("CPU [%u] did not mark itself as running, it could be in undefined state!", i);
                break;
            }

            Pit::earlyDelay(10000); // 10 ms
            readCount++;
        }

        log.info("CPU [%u] is now online!", i);
    }

    Cmos::enableNmi();
    Cpu::enableInterrupts();

    // Free the startup routine page, stack pointer array, gdts array and warm reset vector memory, now that all APs are running.
    // Keep the stacks though, they are not temporary!
    delete[] reinterpret_cast<uint8_t*>(gdtPointers);
    delete[] reinterpret_cast<uint8_t*>(stackPointers);
    delete reinterpret_cast<uint8_t*>(startupCodeRegion);
    delete reinterpret_cast<uint8_t*>(warmResetVectorRegion);
}

void* Apic::prepareApplicationProcessorStacks() {
    // Allocate the stack pointer array
    auto **stacks = reinterpret_cast<uint32_t**>(new uint8_t*[localApics.length()]);

    // Allocate the stacks
    for (uint32_t i = 0; i < localApics.length(); ++i) {
        if (i == LocalApic::getId() || localApics[i] == nullptr) {
            // Skip BSP or disabled processors
            stacks[i] = nullptr;
            continue;
        }

        stacks[i] = reinterpret_cast<uint32_t*>(new uint8_t[applicationProcessorStackSize]);
    }

    return reinterpret_cast<void*>(stacks);
}

void* Apic::prepareApplicationProcessorStartupCode(void *gdts, void *stacks) {
    if (boot_ap_size > Util::PAGESIZE) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Startup code does not fit into one page!");
    }

    // Prepare the empty variables in the startup routine at their original location
    asm volatile("sidt %0"
            : "=m"(boot_ap_idtr));
    asm volatile("mov %%cr0, %%eax;"
            : "=a"(boot_ap_cr0));
    asm volatile("mov %%cr3, %%eax;"
            : "=a"(boot_ap_cr3));
    asm volatile("mov %%cr4, %%eax;"
            : "=a"(boot_ap_cr4));
    boot_ap_gdts = reinterpret_cast<uint32_t>(gdts);
    boot_ap_stacks = reinterpret_cast<uint32_t>(stacks);
    boot_ap_entry = reinterpret_cast<uint32_t>(&applicationProcessorEntry);

    // Allocate physical memory for copying the startup routine
    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    void *startupCodeMemory = memoryService.mapIO(applicationProcessorStartupAddress, Util::PAGESIZE);

    // Identity map the allocated physical memory to the kernel address space (So addresses don't change after enabling paging)
    memoryService.unmap(reinterpret_cast<uint32_t>(startupCodeMemory));
    memoryService.mapPhysicalAddress(applicationProcessorStartupAddress, applicationProcessorStartupAddress, Kernel::Paging::PRESENT | Kernel::Paging::READ_WRITE);

    // Copy the startup routine and prepared variables to the identity mapped page
    const auto startupCode = Util::Address<uint32_t>(reinterpret_cast<uint32_t>(&boot_ap));
    const auto destination = Util::Address<uint32_t>(applicationProcessorStartupAddress);
    destination.copyRange(startupCode, boot_ap_size);

    return reinterpret_cast<void*>(destination.get());
}

void *Apic::prepareApplicationProcessorWarmReset() {
    Cmos::write(0xF, 0x0A); // Shutdown status byte (MPSpec, sec. B.4)

    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    const uint32_t warmResetVectorPhysical = 0x40 << 4 | 0x67;                     // MPSpec, sec. B.4
    void *warmResetVector = memoryService.mapIO(warmResetVectorPhysical, 2, true); // Warm reset vector is DWORD

    // Account for possible misalignment, as mapIO returns a page-aligned pointer
    const uint32_t pageOffset = warmResetVectorPhysical % Util::PAGESIZE;
    const uint32_t warmResetVectorVirtual = reinterpret_cast<uint32_t>(warmResetVector) + pageOffset;

    *reinterpret_cast<volatile uint16_t *>(warmResetVectorVirtual) = applicationProcessorStartupAddress;
    return reinterpret_cast<void *>(warmResetVectorVirtual);
}

void *Apic::prepareApplicationProcessorGdts() {
    // Allocate descriptor pointer array
    auto **gdts = reinterpret_cast<Cpu::Descriptor**>(new Cpu::Descriptor *[localApics.length()]);

    for (uint32_t i = 0; i < localApics.length(); ++i) {
        if (i == LocalApic::getId() || localApics[i] == nullptr) {
            // Skip BSP or disabled processors
            gdts[i] = nullptr;
            continue;
        }

        gdts[i] = allocateApplicationProcessorGdt();
    }

    return reinterpret_cast<void *>(gdts);
}

Cpu::Descriptor *Apic::allocateApplicationProcessorGdt() {
    // Allocate memory for the GDT and TSS. This is never freed, as its used as long as the system runs.
    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();

    auto *gdt = reinterpret_cast<uint16_t*>(memoryService.allocateLowerMemory(48));

    const uint32_t tssSize = sizeof(Kernel::TaskStateSegment);
    auto *tss = reinterpret_cast<void *>(memoryService.allocateLowerMemory(tssSize));

    // Zero everything
    Util::Address<uint32_t>(gdt).setRange(0, 48);
    Util::Address<uint32_t>(tss).setRange(0, tssSize);

    // Set up general GDT for the AP
    // First entry has to be null
    Kernel::System::createGlobalDescriptorTableEntry(gdt, 0, 0, 0, 0, 0);
    // Kernel code segment
    Kernel::System::createGlobalDescriptorTableEntry(gdt, 1, 0, 0xFFFFFFFF, 0x9A, 0xC);
    // Kernel data segment
    Kernel::System::createGlobalDescriptorTableEntry(gdt, 2, 0, 0xFFFFFFFF, 0x92, 0xC);
    // User code segment
    Kernel::System::createGlobalDescriptorTableEntry(gdt, 3, 0, 0xFFFFFFFF, 0xFA, 0xC);
    // User data segment
    Kernel::System::createGlobalDescriptorTableEntry(gdt, 4, 0, 0xFFFFFFFF, 0xF2, 0xC);
    // TSS segment
    Kernel::System::createGlobalDescriptorTableEntry(gdt, 5, reinterpret_cast<uint32_t>(tss), tssSize, 0x89, 0x4);

    return new Cpu::Descriptor {
            .limit = 6 * 8,
            .address = reinterpret_cast<uint32_t>(gdt) // + Kernel::MemoryLayout::KERNEL_START
    };
}

}
