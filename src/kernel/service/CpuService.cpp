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
 */

#include "CpuService.h"
#include "kernel/log/Log.h"
#include "device/cpu/Cpu.h"
#include "TimeService.h"
#include "device/interrupt/apic/Apic.h"
#include "InterruptService.h"
#include "device/cpu/SymmetricMultiprocessing.h"
#include "lib/util/base/Constants.h"
#include "lib/util/base/Address.h"
#include "kernel/memory/MemoryLayout.h"
#include "device/time/rtc/Cmos.h"

namespace Kernel {

uint8_t currentBootingAp = 0;

CpuService::CpuService() {
    // Set virtual CPU id for bootstrap processor
    virtualCpuIds[0] = 0;
    for (uint32_t i = 1; i < 256; i++) {
        virtualCpuIds[i] = 0xff;
    }

    // Allocate GDT and Descriptor arrays for a single core (bootstrap processor)
    gdt = new Kernel::GlobalDescriptorTable[1];
    gdtDescriptor = new Kernel::GlobalDescriptorTable::Descriptor[1];
    tss = new Kernel::GlobalDescriptorTable::TaskStateSegment[1]{};

    gdt[0].addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x9a, 0x0c)); // Kernel code segment
    gdt[0].addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x92, 0x0c)); // Kernel data segment
    gdt[0].addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xfa, 0x0c)); // User code segment
    gdt[0].addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xf2, 0x0c)); // User data segment
    gdt[0].addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(reinterpret_cast<uint32_t>(&tss[0]), sizeof(Kernel::GlobalDescriptorTable::TaskStateSegment), 0x89, 0x04));

    // Store current GDT descriptor in array
    gdtDescriptor[0] = gdt[0].getDescriptor();
}

void CpuService::loadGdt() {
    auto cpuId = getVirtualCpuId();

    // Overwrite TSS entry to make sure it is not marked as busy
    gdt[cpuId].overwriteSegment(5, Kernel::GlobalDescriptorTable::SegmentDescriptor(reinterpret_cast<uint32_t>(&tss[cpuId]), sizeof(Kernel::GlobalDescriptorTable::TaskStateSegment), 0x89, 0x04));

    // Load GDT
    gdt[cpuId].load();

    // Load TSS
    Device::Cpu::loadTaskStateSegment(Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 5));

    // Set segment registers
    Device::Cpu::writeSegmentRegister(Device::Cpu::CS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 1));
    Device::Cpu::writeSegmentRegister(Device::Cpu::SS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::writeSegmentRegister(Device::Cpu::DS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::writeSegmentRegister(Device::Cpu::ES, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::writeSegmentRegister(Device::Cpu::FS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::writeSegmentRegister(Device::Cpu::GS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
}

void CpuService::setTssStackEntry(const uint32_t *stackPointer) {
    auto cpuId = getVirtualCpuId();
    tss[cpuId].esp0 = reinterpret_cast<uint32_t>(stackPointer);
    tss[cpuId].ss0 = static_cast<uint16_t>(Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
}

void CpuService::startupApplicationProcessors() {
    auto &timeService = Kernel::Service::getService<Kernel::TimeService>();
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();

    virtualCpuIds[0] = 0xff;
    prepareGdts();
    prepareStacks();
    prepareApplicationProcessorStartupCode();
    prepareApplicationProcessorWarmReset(); // This is technically only required for discrete APIC, see below

    // Universal Startup Algorithm requires all interrupts disabled (they should be disabled anyway, but disabling them a second time is twice as good)
    Device::Cpu::disableInterrupts();
    Device::Cmos::disableNmi();

    LOG_INFO("CPU [%u] is the bootstrap processor", getVirtualCpuId());

    // Call the startup code on each AP using the INIT-SIPI-SIPI Universal Startup Algorithm
    for (const auto *localApic : interruptService.getApic().getLocalApics()) {
        if (localApic->getCpuId() == getLocalApicId()) {
            // Skip this AP if it's the BSP
            continue;
        }

        currentBootingAp = getVirtualCpuId(localApic->getCpuId());

        // Info on discrete APIC:
        // The INIT IPI is required for CPUs with a discrete APIC, these ignore the STARTUP IPI.
        // For these CPUs, the startup routines address has to be written to the BIOS memory segment
        // (warm reset vector), and the AP has to be configured for warm-reset to start executing there.
        // This is unused for xApic. The INIT IPI is still issued though, to follow the IA-32 manual's
        // "INIT-SIPI-SIPI" sequence and the "universal startup algorithm" (MPSpec, sec. B.4):
        Device::LocalApic::clearErrors();
        // Level-triggered, needs to be...
        Device::LocalApic::sendInitInterProcessorInterrupt(localApic->getCpuId(), Device::LocalApic::InterruptCommandRegisterEntry::Level::ASSERT);
        // xv6 waits 200 us instead.
        Device::LocalApic::waitForInterProcessorInterruptDispatch();
        // ...deasserted manually
        Device::LocalApic::sendInitInterProcessorInterrupt(localApic->getCpuId(), Device::LocalApic::InterruptCommandRegisterEntry::Level::DEASSERT);
        // Not necessary with 10ms delay
        Device::LocalApic::waitForInterProcessorInterruptDispatch();
        // Wait 100 us.
        timeService.busyWait(Util::Time::Timestamp::ofMicroseconds(100));

        // Issue the SIPI twice (for xApic):
        for (uint8_t j = 0; j < 2; ++j) {
            Device::LocalApic::clearErrors();
            Device::LocalApic::sendStartupInterProcessorInterrupt(localApic->getCpuId(), Kernel::MemoryLayout::APPLICATION_PROCESSOR_STARTUP_CODE.startAddress);
            Device::LocalApic::waitForInterProcessorInterruptDispatch();
            timeService.busyWait(Util::Time::Timestamp::ofMicroseconds(200));
        }

        // Wait until the AP marks itself as running, so we can continue to the next one.
        // Because we initializeScene the APs one at a time, runningAPs is not synchronized.
        // If the AP initialization fails (and the system doesn't crash), this will lock the BSP,
        // the same will happen if the SIPI does not reach its target. That's why we abort.
        // Because the system time is not yet functional, we delay to measure the time.
        uint32_t readCount = 0;
        while (!Device::runningApplicationProcessors[localApic->getCpuId()]) {
            if (readCount > 10) {
                // Waited 10 * 10 ms = 0.1 s in total (pretty arbitrarily chosen by me)
                LOG_ERROR("CPU [%u] did not mark itself as running, it could be in undefined state", localApic->getCpuId());
                break;
            }

            timeService.busyWait(Util::Time::Timestamp::ofMilliseconds(10));
            readCount++;
        }

        LOG_INFO("CPU [%u] is now online", localApic->getCpuId());
    }

    Device::Cmos::enableNmi();
    Device::Cpu::enableInterrupts();
}

uint8_t CpuService::getCoreCount() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    return interruptService.usesApic() ? interruptService.getApic().getCoreCount() : 1;
}

uint8_t CpuService::getLocalApicId() {
    if (!Kernel::Service::isServiceRegistered(Kernel::InterruptService::SERVICE_ID)) {
        return 0;
    }

    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    return interruptService.usesApic() ? Device::LocalApic::getId() : 0;
}

uint8_t* CpuService::getStack(uint8_t cpuId) {
    return &stacks[cpuId * Device::AP_STACK_SIZE];
}

void CpuService::prepareGdts() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();

    // Determine CPU count
    auto coreCount = getCoreCount();

    // Keep pointers to bootstrap GDT for freeing the memory later
    auto *bootstrapGdt = gdt;
    auto *bootstrapGdtDescriptor = gdtDescriptor;
    auto *bootstrapTss = tss;

    // Allocate GDT and Descriptor arrays
    gdt = new Kernel::GlobalDescriptorTable[coreCount];
    gdtDescriptor = new Kernel::GlobalDescriptorTable::Descriptor[coreCount];
    tss = new Kernel::GlobalDescriptorTable::TaskStateSegment[coreCount];

    // Create GDTs and Descriptors for each core
    for (const auto *localApic : interruptService.getApic().getLocalApics()) {
        auto virtualCpuId = registerCpu(localApic->getCpuId());
        gdt[virtualCpuId].addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x9a, 0x0c)); // Kernel code segment
        gdt[virtualCpuId].addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x92, 0x0c)); // Kernel data segment
        gdt[virtualCpuId].addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xfa, 0x0c)); // User code segment
        gdt[virtualCpuId].addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xf2, 0x0c)); // User data segment
        gdt[virtualCpuId].addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(reinterpret_cast<uint32_t>(&tss[virtualCpuId]), sizeof(Kernel::GlobalDescriptorTable::TaskStateSegment), 0x89, 0x04));

        // Store current GDT descriptor in array
        gdtDescriptor[virtualCpuId] = gdt[virtualCpuId].getDescriptor();

        // Load GDT on bootstrap processor
        if (localApic->getCpuId() == getLocalApicId()) {
            loadGdt();
        }
    }

    delete bootstrapGdt;
    delete bootstrapGdtDescriptor;
    delete bootstrapTss;
}

void CpuService::prepareStacks() {
    stacks = new uint8_t[getCoreCount() * Device::AP_STACK_SIZE];
    Util::Address(stacks).setRange(0, getCoreCount() * Device::AP_STACK_SIZE);
}

void CpuService::prepareApplicationProcessorStartupCode() {
    if (boot_ap_size > Util::PAGESIZE) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Startup code does not fit into one page!");
    }

    // Prepare the empty variables in the startup routine at their original location
    boot_ap_idt = Kernel::InterruptDescriptorTable::Descriptor::read();
    boot_ap_cr0 = Device::Cpu::readCr0();
    boot_ap_cr3 = reinterpret_cast<uint32_t>(Device::Cpu::readCr3());
    boot_ap_cr4 = Device::Cpu::readCr4();
    boot_ap_virtual_id = reinterpret_cast<uint32_t>(&currentBootingAp);
    boot_ap_gdts = reinterpret_cast<uint32_t>(gdtDescriptor);
    boot_ap_stacks = reinterpret_cast<uint32_t>(stacks);
    boot_ap_entry = reinterpret_cast<uint32_t>(&Device::applicationProcessorEntry);

    // Copy the startup routine and prepared variables to the identity mapped page
    const auto startupCode = Util::Address(reinterpret_cast<uint32_t>(&boot_ap));
    const auto destination = Util::Address(Kernel::MemoryLayout::APPLICATION_PROCESSOR_STARTUP_CODE.startAddress);
    destination.copyRange(startupCode, boot_ap_size);
}

void CpuService::prepareApplicationProcessorWarmReset() {
    Device::Cmos::write(0xF, 0x0A); // Shutdown status byte (MPSpec, sec. B.4)

    auto *warmResetVector = reinterpret_cast<uint16_t*>(0x40 << 4 | 0x67); // MPSpec, sec. B.4

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds" // GCC complains about the reset vector being out of bounds, but it is not (it is just located at a very low address)
    *warmResetVector = Kernel::MemoryLayout::APPLICATION_PROCESSOR_STARTUP_CODE.startAddress;
#pragma GCC diagnostic pop
}

uint8_t CpuService::getVirtualCpuId(uint8_t localApicId) {
    return virtualCpuIds[localApicId];
}

uint8_t CpuService::getVirtualCpuId() {
    return getVirtualCpuId(getLocalApicId());
}

uint8_t CpuService::registerCpu(uint8_t localApicId) {
    virtualCpuIds[localApicId] = virtualCpuCounter++;
    return virtualCpuCounter - 1;
}

}