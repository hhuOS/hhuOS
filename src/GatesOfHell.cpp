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

#include "kernel/memory/GlobalDescriptorTable.h"
#include "device/cpu/Cpu.h"
#include "kernel/log/Logger.h"
#include "GatesOfHell.h"
#include "kernel/paging/MemoryLayout.h"
#include "kernel/paging/Paging.h"
#include "kernel/memory/PagingAreaManager.h"
#include "kernel/memory/PageFrameAllocator.h"
#include "kernel/paging/VirtualAddressSpace.h"
#include "kernel/service/MemoryService.h"
#include "kernel/interrupt/InterruptDescriptorTable.h"
#include "kernel/service/InterruptService.h"
#include "device/interrupt/apic/Apic.h"

extern const uint32_t ___KERNEL_DATA_START__;
extern const uint32_t ___KERNEL_DATA_END__;

const uint32_t KERNEL_DATA_START = reinterpret_cast<uint32_t>(&___KERNEL_DATA_START__);
const uint32_t KERNEL_DATA_END = reinterpret_cast<uint32_t>(&___KERNEL_DATA_END__);

const constexpr uint32_t INITIAL_PAGING_AREA_SIZE = 32 * 1024;
const constexpr uint32_t INITIAL_KERNEL_HEAP_SIZE = 128 * 1024;

extern "C" {
    void _init();
}

namespace Device {
class Machine;
}  // namespace Device

Kernel::Logger GatesOfHell::log = Kernel::Logger::get("GatesOfHell");
Kernel::GlobalDescriptorTable GatesOfHell::gdt{};
Kernel::GlobalDescriptorTable::TaskStateSegment GatesOfHell::tss{};
Util::HeapMemoryManager *GatesOfHell::kernelHeap = nullptr;

extern "C" void start(uint32_t multibootMagic, const Kernel::Multiboot *multiboot) {
    GatesOfHell::enter(multibootMagic, *multiboot);
}

void GatesOfHell::enter(uint32_t multibootMagic, const Kernel::Multiboot &multiboot) {
    // Initialize GDT
    gdt = Kernel::GlobalDescriptorTable();
    gdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x9a, 0x0c)); // Kernel code segment
    gdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x92, 0x0c)); // Kernel data segment
    gdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xfa, 0x0c)); // User code segment
    gdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xf2, 0x0c)); // User data segment
    gdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(reinterpret_cast<uint32_t>(&tss), sizeof(Kernel::GlobalDescriptorTable::TaskStateSegment), 0x89, 0x04));
    gdt.load();

    Device::Cpu::setSegmentRegister(Device::Cpu::CS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 1));
    Device::Cpu::setSegmentRegister(Device::Cpu::DS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::ES, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::FS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::GS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::SS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));

    // Scan memory map
    auto *memoryMap = &multiboot.getTag<Kernel::Multiboot::MemoryMapHeader>(Kernel::Multiboot::MEMORY_MAP);
    auto memoryMapEntries = (memoryMap->tagHeader.size - sizeof(Kernel::Multiboot::TagHeader)) / memoryMap->entrySize;

    // Search for available memory to bootstrap paging
    uint32_t physicalMemoryLimit = 0;
    uint32_t bootstrapMemory = 0;
    for (uint32_t i = 0; i < memoryMapEntries; i++) {
        auto currentAddress = reinterpret_cast<uint32_t>(memoryMap) + sizeof(Kernel::Multiboot::MemoryMapHeader) + i * memoryMap->entrySize;
        auto &entry = *reinterpret_cast<Kernel::Multiboot::MemoryMapEntry*>(currentAddress);

        if (entry.address + entry.length > physicalMemoryLimit) {
            physicalMemoryLimit = entry.address + entry.length;
        }

        if (bootstrapMemory == 0 && (entry.address > KERNEL_DATA_END && entry.length >= INITIAL_PAGING_AREA_SIZE + INITIAL_KERNEL_HEAP_SIZE)) {
            bootstrapMemory = entry.address;
            entry.address += INITIAL_PAGING_AREA_SIZE + INITIAL_KERNEL_HEAP_SIZE;
        }
    }

    if (bootstrapMemory == 0) {
        Util::Exception::throwException(Util::Exception::OUT_OF_MEMORY, "No memory available for bootstrapping!");
    }

    uint32_t pagingAreaPhysical = bootstrapMemory;
    uint32_t kernelHeapPhysical = bootstrapMemory + INITIAL_PAGING_AREA_SIZE;

    // Create page directory
    auto *pageDirectory = reinterpret_cast<Kernel::Paging::Table*>(pagingAreaPhysical);
    pageDirectory->clear();

    // Page tables will be allocated in bootstrap memory, directly after the page directory
    auto *pageTableMemory = reinterpret_cast<Kernel::Paging::Table*>(pagingAreaPhysical + sizeof(Kernel::Paging::Table));

    // Create identity mapping for kernel
    const auto kernelSize = Util::Address<uint32_t>(KERNEL_DATA_END - KERNEL_DATA_START).alignUp(Kernel::Paging::PAGESIZE).get();
    pageTableMemory += createInitialMapping(*pageDirectory, pageTableMemory, KERNEL_DATA_START, KERNEL_DATA_START, kernelSize / Kernel::Paging::PAGESIZE);

    // Map beginning of paging area
    const auto pagingAreaVirtual = Util::Address<uint32_t>(KERNEL_DATA_END).alignUp(Kernel::Paging::PAGESIZE).get();
    pageTableMemory += createInitialMapping(*pageDirectory, pageTableMemory, pagingAreaPhysical, pagingAreaVirtual, INITIAL_PAGING_AREA_SIZE / Kernel::Paging::PAGESIZE);

    // Map beginning of kernel heap
    const auto kernelHeapVirtual = pagingAreaVirtual + Kernel::MemoryLayout::PAGING_AREA_SIZE;
    pageTableMemory += createInitialMapping(*pageDirectory, pageTableMemory, kernelHeapPhysical, kernelHeapVirtual, INITIAL_KERNEL_HEAP_SIZE / Kernel::Paging::PAGESIZE);

    // Copy memory map to end of initial kernel heap area (will be overwritten later on!)
    auto memoryMapTarget = Util::Address<uint32_t>((kernelHeapPhysical + INITIAL_KERNEL_HEAP_SIZE) - memoryMap->tagHeader.size);
    memoryMapTarget.copyRange(Util::Address<uint32_t>(memoryMap), memoryMap->tagHeader.size);
    memoryMap = reinterpret_cast<Kernel::Multiboot::MemoryMapHeader*>((kernelHeapVirtual + INITIAL_KERNEL_HEAP_SIZE) - memoryMap->tagHeader.size);

    // Enable paging
    Kernel::Paging::loadDirectory(*pageDirectory);
    Device::Cpu::writeCr0(Device::Cpu::readCr0() | Device::Cpu::PAGING);

    // Initialize kernel heap
    static Util::FreeListMemoryManager kernelHeapManager;
    kernelHeapManager.initialize(reinterpret_cast<uint8_t*>(kernelHeapVirtual), reinterpret_cast<uint8_t*>(Kernel::MemoryLayout::KERNEL_HEAP_END_ADDRESS));
    kernelHeap = &kernelHeapManager;

    // Initialize static data structures
    _init();

    // Initialize Paging Area Manager -> Manages the virtual addresses of all page tables and directories
    auto usedPagingAreaPages = (reinterpret_cast<uint32_t>(pageTableMemory) - pagingAreaPhysical) / Kernel::Paging::PAGESIZE;
    auto *pagingAreaManager = new Kernel::PagingAreaManager(reinterpret_cast<uint8_t*>(pagingAreaVirtual), INITIAL_PAGING_AREA_SIZE / Kernel::Paging::PAGESIZE, usedPagingAreaPages);

    // Initialize page frame allocator -> Manages physical memory
    auto *pageFrameAllocator = new Kernel::PageFrameAllocator(*pagingAreaManager, nullptr, reinterpret_cast<uint8_t*>(physicalMemoryLimit - 1));
    // Reserve low memory (used by BIOS and ISA devices)
    pageFrameAllocator->setMemory(nullptr, reinterpret_cast<uint8_t*>(0x100000), 0, true);
    // Reserve kernel
    pageFrameAllocator->setMemory(reinterpret_cast<uint8_t*>(KERNEL_DATA_START), reinterpret_cast<uint8_t *>(KERNEL_DATA_START + kernelSize), 0, true);

    // Reserve corresponding block in memory map
    for (uint32_t i = 0; i < memoryMapEntries; i++) {
        auto currentAddress = reinterpret_cast<uint32_t>(memoryMap) + sizeof(Kernel::Multiboot::MemoryMapHeader) + i * memoryMap->entrySize;
        auto &entry = *reinterpret_cast<Kernel::Multiboot::MemoryMapEntry*>(currentAddress);

        if (entry.type == Kernel::Multiboot::RESERVED) {
            pageFrameAllocator->setMemory(reinterpret_cast<uint8_t*>(entry.address), reinterpret_cast<uint8_t*>(entry.address + entry.length), 0, true);
        }
    }

    // Create kernel address space and memory service
    auto *addressSpace = new Kernel::VirtualAddressSpace(*pageDirectory, *kernelHeap);
    auto *memoryService = new Kernel::MemoryService(pageFrameAllocator, pagingAreaManager, addressSpace);

    // Create interrupt service with PIC or APIC, depending on what is available
    Kernel::InterruptService *interruptService;
    if (Device::Apic::isAvailable()) {
        log.info("APIC detected");
        auto *apic = Device::Apic::initialize();
        if (apic == nullptr) {
            auto *pic = new Device::Pic();
            interruptService = new Kernel::InterruptService(pic);
            log.warn("Failed to initialize APIC -> Falling back to PIC");
        } else {
            interruptService = new Kernel::InterruptService(apic);
        }

        if (apic != nullptr && apic->isSymmetricMultiprocessingSupported()) {
            apic->startupApplicationProcessors();
        }
    } else {
        log.info("APIC not available -> Falling back to PIC");
        auto *pic = new Device::Pic();
        interruptService = new Kernel::InterruptService(pic);
    }

    // Memory and interrupt services are initialized -> Page faults can now be handled
    Kernel::Service::registerService(Kernel::MemoryService::SERVICE_ID, memoryService);
    Kernel::Service::registerService(Kernel::InterruptService::SERVICE_ID, interruptService);
    log.info("Welcome to hhuOS!");

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Once you entered the gates of hell, you are not allowed to leave!");
}

Util::HeapMemoryManager& GatesOfHell::getKernelHeap() {
    return *kernelHeap;
}

uint32_t GatesOfHell::createInitialMapping(Kernel::Paging::Table &pageDirectory, Kernel::Paging::Table *pageTableMemory, uint32_t physicalStartAddress, uint32_t virtualStartAddress, uint32_t pageCount) {
    uint32_t allocatedPageTables = 0;

    for (uint32_t i = 0; i <= pageCount; i++) {
        auto physicalAddress = physicalStartAddress + i * Kernel::Paging::PAGESIZE;
        auto virtualAddress = virtualStartAddress + i * Kernel::Paging::PAGESIZE;

        auto directoryIndex = Kernel::Paging::DIRECTORY_INDEX(virtualAddress);
        if (pageDirectory[directoryIndex].isUnused()) {
            // Allocate new page table if necessary
            auto &pageTable = *pageTableMemory++;
            pageTable.clear();

            pageDirectory[directoryIndex].set(reinterpret_cast<uint32_t>(&pageTable), Kernel::Paging::PRESENT | Kernel::Paging::WRITABLE);
            allocatedPageTables++;
        }

        auto &table = *reinterpret_cast<Kernel::Paging::Table*>(pageDirectory[directoryIndex].getAddress());
        auto tableIndex = Kernel::Paging::TABLE_INDEX(virtualAddress);

        // Create identity mapping for current kernel frame
        table[tableIndex].set(physicalAddress, Kernel::Paging::PRESENT | Kernel::Paging::WRITABLE);
    }

    return allocatedPageTables;
}
