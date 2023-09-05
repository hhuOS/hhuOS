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

#include "LocalApic.h"

#include "lib/util/hardware/CpuId.h"
#include "kernel/system/System.h"
#include "kernel/service/InterruptService.h"
#include "lib/util/base/Constants.h"
#include "kernel/service/MemoryService.h"
#include "device/cpu/IoPort.h"
#include "device/cpu/ModelSpecificRegister.h"
#include "kernel/interrupt/InterruptVector.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/Iterator.h"

namespace Device {
enum InterruptRequest : uint8_t;

uint32_t LocalApic::mmioAddress = 0;

ModelSpecificRegister LocalApic::ia32ApicBaseMsr = ModelSpecificRegister(0x1b);
Util::Array<LocalApic::Register> LocalApic::lintRegs = Util::Array<Register>({  // Local interrupt to register offset lookup.
    static_cast<Register>(0x2F0), static_cast<Register>(0x320),
    static_cast<Register>(0x330), static_cast<Register>(0x340),
    static_cast<Register>(0x350), static_cast<Register>(0x360),
    static_cast<Register>(0x370)});

Util::Async::Spinlock LocalApic::commandLock = Util::Async::Spinlock();

LocalApic::LocalApic(uint8_t cpuId) : cpuId(cpuId) {}

bool LocalApic::supportsXApic() {
    return (Util::Hardware::CpuId::getCpuFeatureBits() & Util::Hardware::CpuId::APIC) != 0;
}

bool LocalApic::supportsX2Apic() {
    return (Util::Hardware::CpuId::getCpuFeatureBits() & Util::Hardware::CpuId::X2APIC) != 0;
}

uint8_t LocalApic::getId() {
    return readDoubleWord(ID) >> 24;
}

uint8_t LocalApic::getVersion() {
    return readDoubleWord(VER) & 0xff;
}

void LocalApic::initialize() {
    // Mask all local interrupt sources
    initializeLocalVectorTable();

    // Configure the non maskable interrupt pin.
    // This is usually LINT1, edge-triggered and active-high, but ACPI reports this in case of deviations.
    for (const auto &nmiSource : nmiSources) {
        LocalVectorTableEntry lvtEntry{};
        lvtEntry.vector = static_cast<Kernel::InterruptVector>(0); // NMI doesn't have vector
        lvtEntry.deliveryMode = LocalVectorTableEntry::DeliveryMode::NMI;
        lvtEntry.pinPolarity = nmiSource.polarity;
        lvtEntry.triggerMode = nmiSource.trigger;
        lvtEntry.isMasked = false;
        writeLocalVectorTable(nmiSource.source, lvtEntry);
    }

    // SW Enable APIC by setting the Spurious Interrupt Vector Register with spurious vector number 0xFF
    // and the SW ENABLE flag.
    SpuriousVectorRegisterEntry svrEntry{};
    svrEntry.vector = Kernel::InterruptVector::SPURIOUS;
    svrEntry.isSwEnabled = true;
    svrEntry.suppressEoiBroadcasting = false; // EOI level triggered external interrupts through the local APIC
    writeSpuriousVectorRegister(svrEntry);

    // Clear outstanding stuff. Errors are not cleared here, because this would arm the error interrupt.
    // This is done when plugging in the error interrupt instead.
    sendEndOfInterrupt();

    // Synchronize the arbitration IDs (IA-32, sec. 3.11.7)
    // Multiple APICs communicate with each other. To select which message is handled first when multiple
    // messages occur at a similar time, an arbitration mechanism is used, which uses an arbitration ID based
    // on the local APIC ID. In P6 family processors, the arbitration IDs can be synchronized with the APIC IDs
    // by sending the INIT-level-deassert IPI to all APICs.
    synchronizeArbitrationIds();

    // Allow all interrupts to be forwarded to the CPU by setting the Task-Priority Class and Sub Class thresholds to 0
    // This should be 0 after power-up, but it doesn't hurt to set it again
    writeDoubleWord(TPR, 0);
}

void LocalApic::enableXApicMode(uint32_t baseAddress) {
    // Mask all PIC interrupts that have been enabled previously. After the APIC has been initialized, the
    // InterruptService only reaches the I/O APIC's REDTBL registers.
    // At this point, no PIC interrupts should be unmasked, plugging in interrupt handlers should
    // be done after the APIC is initialized, otherwise they will be lost!
    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();
    for (uint8_t i = 0; i < 16; ++i) {
        interruptService.forbidHardwareInterrupt(static_cast<InterruptRequest>(i));
    }

    // Physically connect the APIC to the BSP, just in case the IMCR actually exists
    IoPort(0x22).writeByte(0x70); // Select IMCR at 0x70
    IoPort(0x23).writeByte(0x01); // Write IMCR, 0x00 connects PIC to LINT0, 0x01 disconnects

    // The memory allocated here is never freed, because this implementation does not support disabling the APIC after enabling it.
    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    void *virtualAddress = memoryService.mapIO(baseAddress, Util::PAGESIZE);

    // Account for possible misalignment, as mapIO returns a page-aligned pointer
    const uint32_t pageOffset = baseAddress % Util::PAGESIZE;
    mmioAddress = reinterpret_cast<uint32_t>(virtualAddress) + pageOffset;
}

uint32_t LocalApic::readDoubleWord(LocalApic::Register reg) {
    return *reinterpret_cast<uint32_t*>(mmioAddress + reg);
}

void LocalApic::writeDoubleWord(LocalApic::Register reg, uint32_t value) {
    *reinterpret_cast<uint32_t *>(mmioAddress + reg) = value;
}

void LocalApic::synchronizeArbitrationIds() {
    InterruptCommandRegisterEntry icrEntry{};
    icrEntry.vector = static_cast<Kernel::InterruptVector>(0);
    icrEntry.deliveryMode = InterruptCommandRegisterEntry::DeliveryMode::INIT;
    icrEntry.destinationMode = InterruptCommandRegisterEntry::DestinationMode::PHYSICAL;
    icrEntry.level = InterruptCommandRegisterEntry::Level::DEASSERT;
    icrEntry.triggerMode = InterruptCommandRegisterEntry::TriggerMode::LEVEL;
    icrEntry.destinationShorthand = InterruptCommandRegisterEntry::DestinationShorthand::ALL;
    icrEntry.destination = 0;
    writeInterruptCommandRegister(icrEntry);
    waitForInterProcessorInterruptDispatch();
}

void LocalApic::initializeLocalVectorTable() {
    // Default values
    LocalVectorTableEntry lvtEntry{};
    lvtEntry.deliveryMode = LocalVectorTableEntry::DeliveryMode::FIXED;
    lvtEntry.pinPolarity = LocalVectorTableEntry::PinPolarity::HIGH;
    lvtEntry.triggerMode = LocalVectorTableEntry::TriggerMode::EDGE;
    lvtEntry.isMasked = true;

    // Set all the vector numbers
    // lvtEntry.vector = Kernel::InterruptVector::CMCI;
    // writeLocalVectorTable(CMCI, lvtEntry); // The CMCI only exists on modern CPUs
    lvtEntry.vector = Kernel::InterruptVector::APICTIMER;
    writeLocalVectorTable(TIMER, lvtEntry);
    lvtEntry.vector = Kernel::InterruptVector::THERMAL;
    writeLocalVectorTable(THERMAL, lvtEntry);
    lvtEntry.vector = Kernel::InterruptVector::PERFORMANCE;
    writeLocalVectorTable(PERFORMANCE, lvtEntry);
    lvtEntry.vector = Kernel::InterruptVector::LINT0;
    writeLocalVectorTable(LINT0, lvtEntry);
    lvtEntry.vector = Kernel::InterruptVector::LINT1;
    writeLocalVectorTable(LINT1, lvtEntry);
    lvtEntry.vector = Kernel::InterruptVector::ERROR;
    writeLocalVectorTable(ERROR, lvtEntry);
}

void LocalApic::addNonMaskableInterrupt(Device::LocalApic::LocalInterrupt nmiLint, LocalVectorTableEntry::PinPolarity nmiPolarity, LocalVectorTableEntry::TriggerMode nmiTrigger) {
    nmiSources.add({nmiLint, nmiPolarity, nmiTrigger});
}

LocalApic::BaseModelSpecificRegisterEntry LocalApic::readBaseModelSpecificRegister() {
    return static_cast<BaseModelSpecificRegisterEntry>(ia32ApicBaseMsr.readQuadWord()); // Atomic read
}

void LocalApic::writeBaseModelSpecificRegister(const LocalApic::BaseModelSpecificRegisterEntry &msrEntry) {
    ia32ApicBaseMsr.writeQuadWord(static_cast<uint64_t>(msrEntry)); // Atomic write
}

LocalApic::SpuriousVectorRegisterEntry LocalApic::readSpuriousVectorRegister() {
    return SpuriousVectorRegisterEntry(readDoubleWord(SVR));
}

void LocalApic::writeSpuriousVectorRegister(const LocalApic::SpuriousVectorRegisterEntry &svrEntry) {
    writeDoubleWord(SVR, static_cast<uint32_t>(svrEntry));
}

LocalApic::LocalVectorTableEntry LocalApic::readLocalVectorTable(LocalApic::LocalInterrupt lint) {
    return LocalVectorTableEntry(readDoubleWord(lintRegs[lint]));
}

void LocalApic::writeLocalVectorTable(LocalApic::LocalInterrupt lint, const LocalApic::LocalVectorTableEntry &lvtEntry) {
    writeDoubleWord(lintRegs[lint], static_cast<uint32_t>(lvtEntry));
}

LocalApic::InterruptCommandRegisterEntry LocalApic::readInterruptCommandRegister() {
    commandLock.acquire(); // This needs to be synchronized in case multiple APs issue IPIs
    const uint32_t low = readDoubleWord(ICR_LOW);
    const uint64_t high = readDoubleWord(ICR_HIGH);
    commandLock.release();

    return InterruptCommandRegisterEntry(low | high << 32);

}

void LocalApic::writeInterruptCommandRegister(const LocalApic::InterruptCommandRegisterEntry &icrEntry) {
    auto value = static_cast<uint64_t>(icrEntry);

    commandLock.acquire(); // This needs to be synchronized in case multiple APs issue IPIs
    writeDoubleWord(ICR_HIGH, value >> 32);
    writeDoubleWord(ICR_LOW, value & 0xFFFFFFFF); // Writing the low DW sends the IPI
    commandLock.release();
}

void LocalApic::allow(LocalApic::LocalInterrupt lint) {
    LocalVectorTableEntry entry = readLocalVectorTable(lint);
    entry.isMasked = false;
    writeLocalVectorTable(lint, entry);
}

void LocalApic::forbid(LocalApic::LocalInterrupt lint) {
    LocalVectorTableEntry entry = readLocalVectorTable(lint);
    entry.isMasked = true;
    writeLocalVectorTable(lint, entry);
}

bool LocalApic::status(LocalApic::LocalInterrupt lint) {
    return readLocalVectorTable(lint).isMasked;
}

void LocalApic::sendEndOfInterrupt() {
    // This works for multiple cores because the core that handles the interrupt calls this function and thus reaches the correct local APIC
    writeDoubleWord(EOI, 0);
}

void LocalApic::clearErrors() {
    // Clear possible error interrupts (write twice because ESR is read/write register, writing once does not
    // change a subsequently read value, in fact the register should always be written once before reading)
    writeDoubleWord(ESR, 0);
    writeDoubleWord(ESR, 0);
}

void LocalApic::sendInitInterProcessorInterrupt(uint8_t id, LocalApic::InterruptCommandRegisterEntry::Level level) {
    InterruptCommandRegisterEntry icrEntry{};
    icrEntry.vector = static_cast<Kernel::InterruptVector>(0); // INIT should have vector number 0
    icrEntry.deliveryMode = InterruptCommandRegisterEntry::DeliveryMode::INIT;
    icrEntry.destinationMode = InterruptCommandRegisterEntry::DestinationMode::PHYSICAL;
    icrEntry.level = level; // ASSERT or DEASSERT
    icrEntry.triggerMode = InterruptCommandRegisterEntry::TriggerMode::LEVEL;
    icrEntry.destinationShorthand = InterruptCommandRegisterEntry::DestinationShorthand::NO; // Only broadcast to CPU in destination field
    icrEntry.destination = id;
    writeInterruptCommandRegister(icrEntry); // Writing ICR issues IPI
}

void LocalApic::sendStartupInterProcessorInterrupt(uint8_t id, uint32_t startupCodeAddress) {
    InterruptCommandRegisterEntry icrEntry{};
    icrEntry.vector = static_cast<Kernel::InterruptVector>(startupCodeAddress >> 12); // Startup code physical page
    icrEntry.deliveryMode = InterruptCommandRegisterEntry::DeliveryMode::STARTUP;
    icrEntry.destinationMode = InterruptCommandRegisterEntry::DestinationMode::PHYSICAL;
    icrEntry.level = InterruptCommandRegisterEntry::Level::ASSERT;
    icrEntry.triggerMode = InterruptCommandRegisterEntry::TriggerMode::EDGE;
    icrEntry.destinationShorthand = InterruptCommandRegisterEntry::DestinationShorthand::NO;
    icrEntry.destination = id;
    writeInterruptCommandRegister(icrEntry); // Writing ICR issues IPI
}

void LocalApic::waitForInterProcessorInterruptDispatch() {
    do {
        // Spinloop: Pause prevents speculative memory reads, memory prevents compiler memory reordering,
        //           so the ICR polls (simple memory reads after all) should happen as intended.
        // I am actually not sure if this is necessary, since the MMIO read address is marked volatile?
        asm volatile("pause"
                :
                :
                : "memory");
    } while (readInterruptCommandRegister().deliveryStatus == InterruptCommandRegisterEntry::DeliveryStatus::PENDING);
}

uint8_t LocalApic::getCpuId() const {
    return cpuId;
}

LocalApic::BaseModelSpecificRegisterEntry::BaseModelSpecificRegisterEntry(uint64_t registerValue) :
        isBootstrapProcessor(registerValue & (1 << 8)),
        isX2Apic(registerValue & (1 << 10)),
        isXApic(registerValue & (1 << 11)),
        baseField(registerValue & 0xfffff000) {}

LocalApic::BaseModelSpecificRegisterEntry::operator uint64_t() const {
    return static_cast<uint64_t>(isBootstrapProcessor) << 8 | static_cast<uint64_t>(isX2Apic) << 10 | static_cast<uint64_t>(isXApic) << 11 | static_cast<uint64_t>(baseField) << 12;
}

LocalApic::SpuriousVectorRegisterEntry::SpuriousVectorRegisterEntry(uint32_t registerValue) :
    vector(static_cast<Kernel::InterruptVector>(registerValue & 0xFF)),
    isSwEnabled(registerValue & (1 << 8)),
    hasFocusProcessorChecking(registerValue & (1 << 9)),
    suppressEoiBroadcasting(registerValue & (1 << 12)) {}

LocalApic::SpuriousVectorRegisterEntry::operator uint32_t() const {
    return static_cast<uint32_t>(vector) | static_cast<uint32_t>(isSwEnabled) << 8 | static_cast<uint32_t>(hasFocusProcessorChecking) << 9 | static_cast<uint32_t>(suppressEoiBroadcasting) << 12;
}

LocalApic::LocalVectorTableEntry::LocalVectorTableEntry(uint32_t registerValue) :
    vector(static_cast<Kernel::InterruptVector>(registerValue & 0xFF)),
    deliveryMode(static_cast<DeliveryMode>((registerValue & (0b111 << 8)) >> 8)),
    deliveryStatus(static_cast<DeliveryStatus>((registerValue & (1 << 12)) >> 12)),
    pinPolarity(static_cast<PinPolarity>((registerValue & (1 << 13)) >> 13)),
    triggerMode(static_cast<TriggerMode>((registerValue & (1 << 15)) >> 15)),
    isMasked(registerValue & (1 << 16)),
    timerMode(static_cast<TimerMode>((registerValue & (0b11 << 17)) >> 17)) {}


LocalApic::LocalVectorTableEntry::operator uint32_t() const {
    return static_cast<uint32_t>(vector) | static_cast<uint32_t>(deliveryMode) << 8 | static_cast<uint32_t>(pinPolarity) << 13 | static_cast<uint32_t>(triggerMode) << 15 | static_cast<uint32_t>(isMasked) << 16 | static_cast<uint32_t>(timerMode) << 17;
}

LocalApic::InterruptCommandRegisterEntry::InterruptCommandRegisterEntry(uint64_t registerValue) :
    vector(static_cast<Kernel::InterruptVector>(registerValue & 0xFF)),
    deliveryMode(static_cast<DeliveryMode>((registerValue & (0b111 << 8)) >> 8)),
    destinationMode(static_cast<DestinationMode>((registerValue & (1 << 11)) >> 11)),
    deliveryStatus(static_cast<DeliveryStatus>((registerValue & (1 << 12)) >> 12)),
    level(static_cast<Level>((registerValue & (1 << 14)) >> 14)),
    triggerMode(static_cast<TriggerMode>((registerValue & (1 << 15)) >> 15)),
    destinationShorthand(static_cast<DestinationShorthand>((registerValue & (0b11 << 18)) >> 18)),
    destination(registerValue >> 56) {}


LocalApic::InterruptCommandRegisterEntry::operator uint64_t() const {
    return static_cast<uint64_t>(vector) | static_cast<uint64_t>(deliveryMode) << 8 | static_cast<uint64_t>(destinationMode) << 11 | static_cast<uint64_t>(deliveryStatus) << 12 | static_cast<uint64_t>(level) << 14 | static_cast<uint64_t>(triggerMode) << 15 | static_cast<uint64_t>(destinationShorthand) << 18 | static_cast<uint64_t>(destination) << 56;
}

bool LocalApic::NmiSource::operator!=(const LocalApic::NmiSource &other) const {
    return source != other.source || polarity != other.polarity || trigger != other.trigger;
}

}