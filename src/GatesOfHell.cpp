/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 */

#include "device/cpu/GlobalDescriptorTable.h"
#include "device/cpu/Cpu.h"
#include "kernel/log/Logger.h"
#include "GatesOfHell.h"
#include "kernel/paging/MemoryLayout.h"

extern const uint32_t ___KERNEL_DATA_END__;
const uint32_t KERNEL_DATA_END = reinterpret_cast<uint32_t>(&___KERNEL_DATA_END__);

extern "C" {
    void _init();
}

namespace Device {
class Machine;
}  // namespace Device

Kernel::Logger GatesOfHell::log = Kernel::Logger::get("GatesOfHell");
Device::GlobalDescriptorTable GatesOfHell::gdt{};
Device::GlobalDescriptorTable::TaskStateSegment GatesOfHell::tss{};
Util::HeapMemoryManager *GatesOfHell::kernelHeap = nullptr;

extern "C" void start(uint32_t multibootMagic, const Kernel::Multiboot *multiboot) {
    GatesOfHell::enter(multibootMagic, *multiboot);
}

void GatesOfHell::enter(uint32_t multibootMagic, const Kernel::Multiboot &multiboot) {
    // Initialize GDT
    gdt.addSegment(Device::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x9a, 0x0c)); // Kernel code segment
    gdt.addSegment(Device::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x92, 0x0c)); // Kernel data segment
    gdt.addSegment(Device::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xfa, 0x0c)); // User code segment
    gdt.addSegment(Device::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xf2, 0x0c)); // User data segment
    gdt.addSegment(Device::GlobalDescriptorTable::SegmentDescriptor(reinterpret_cast<uint32_t>(&tss), sizeof(Device::GlobalDescriptorTable::TaskStateSegment), 0x89, 0x04));
    gdt.load();

    Device::Cpu::setSegmentRegister(Device::Cpu::CS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 1));
    Device::Cpu::setSegmentRegister(Device::Cpu::DS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::ES, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::FS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::GS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::SS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));

    // Scan memory map
    const auto &memoryMap = multiboot.getTag<Kernel::Multiboot::MemoryMapHeader>(Kernel::Multiboot::MEMORY_MAP);
    auto mapEntries = (memoryMap.tagHeader.size - sizeof(Kernel::Multiboot::TagHeader)) / memoryMap.entrySize;

    // Search for initial heap area
    for (uint32_t i = 0; i < mapEntries; i++) {
        auto currentAddress = reinterpret_cast<uint32_t>(&memoryMap) + sizeof(Kernel::Multiboot::MemoryMapHeader) + i * memoryMap.entrySize;
        auto &entry = *reinterpret_cast<Kernel::Multiboot::MemoryMapEntry*>(currentAddress);

        if (entry.address > KERNEL_DATA_END && entry.length >= 0x100000) {
            static Util::FreeListMemoryManager kernelHeapManager;
            kernelHeapManager.initialize(reinterpret_cast<uint8_t*>(entry.address), reinterpret_cast<uint8_t*>(Kernel::MemoryLayout::KERNEL_HEAP_END_ADDRESS));
            kernelHeap = &kernelHeapManager;

            entry.address += 0x100000;
            break;
        }
    }

    // Initialize static data structures
    _init();
    auto usedMemory = kernelHeap->getTotalMemory() - kernelHeap->getFreeMemory();

    while (true) {}

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Once you entered the gates of hell, you are not allowed to leave!");
}

Util::HeapMemoryManager& GatesOfHell::getKernelHeap() {
    return *kernelHeap;
}
