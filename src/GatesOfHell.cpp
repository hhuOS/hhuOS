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
#include "kernel/memory/MemoryLayout.h"
#include "kernel/memory/Paging.h"
#include "kernel/memory/PagingAreaManager.h"
#include "kernel/memory/PageFrameAllocator.h"
#include "kernel/memory/VirtualAddressSpace.h"
#include "kernel/service/MemoryService.h"
#include "kernel/interrupt/InterruptDescriptorTable.h"
#include "kernel/service/InterruptService.h"
#include "device/interrupt/apic/Apic.h"
#include "device/system/Acpi.h"
#include "kernel/service/InformationService.h"
#include "device/system/SmBios.h"

extern const uint32_t ___KERNEL_DATA_START__;
extern const uint32_t ___KERNEL_DATA_END__;

const uint32_t KERNEL_DATA_START = reinterpret_cast<uint32_t>(&___KERNEL_DATA_START__);
const uint32_t KERNEL_DATA_END = reinterpret_cast<uint32_t>(&___KERNEL_DATA_END__);

const constexpr uint32_t INITIAL_PAGING_AREA_SIZE = 128 * 1024;
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
bool GatesOfHell::memoryManagementInitialized = false;

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
    auto multibootSize = multiboot.getSize();
    auto *memoryMap = &multiboot.getTag<Kernel::Multiboot::MemoryMapHeader>(Kernel::Multiboot::MEMORY_MAP);
    auto memoryMapEntries = (memoryMap->tagHeader.size - sizeof(Kernel::Multiboot::TagHeader)) / memoryMap->entrySize;

    // Search for available memory to bootstrap paging
    uint32_t physicalMemoryLimit = 0;
    uint32_t bootstrapMemory = 0;
    for (uint32_t i = 0; i < memoryMapEntries; i++) {
        auto currentAddress = reinterpret_cast<uint32_t>(memoryMap) + sizeof(Kernel::Multiboot::MemoryMapHeader) + i * memoryMap->entrySize;
        auto &entry = *reinterpret_cast<Kernel::Multiboot::MemoryMapEntry*>(currentAddress);

        if (entry.type == Kernel::Multiboot::AVAILABLE && (entry.address + entry.length) > physicalMemoryLimit) {
            physicalMemoryLimit = entry.address + entry.length;
        }

        if (bootstrapMemory == 0 && entry.type == Kernel::Multiboot::AVAILABLE) {
            auto alignedAddress = Util::Address<uint32_t>(entry.address).alignUp(Kernel::Paging::PAGESIZE).get();
            auto alignedLength = entry.length - (alignedAddress - entry.address);

            if (alignedAddress > KERNEL_DATA_END && alignedLength >= INITIAL_PAGING_AREA_SIZE + INITIAL_KERNEL_HEAP_SIZE) {
                // Entry starts and ends above kernel
                bootstrapMemory = alignedAddress;
                entry.address = alignedAddress + INITIAL_PAGING_AREA_SIZE + INITIAL_KERNEL_HEAP_SIZE;
            } else if (alignedAddress + alignedLength > KERNEL_DATA_END) {
                // Entry starts before or within kernel, but ends above kernel
                auto alignedKernelEndAddress = Util::Address<uint32_t>(KERNEL_DATA_END).alignUp(Kernel::Paging::PAGESIZE).get();
                auto usableLength = alignedLength - (alignedKernelEndAddress - alignedAddress);
                if (usableLength >= INITIAL_PAGING_AREA_SIZE + INITIAL_KERNEL_HEAP_SIZE) {
                    bootstrapMemory = alignedKernelEndAddress;
                    entry.address = bootstrapMemory + INITIAL_PAGING_AREA_SIZE + INITIAL_KERNEL_HEAP_SIZE;
                }
            }
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

    // Create identity mapping for lower memory (1 MiB), used for ISA DMA and BIOS related stuff
    // Depending on the system and bootloader, this may be necessary to initialize ACPI and SMBIOS data structures.
    // This also necessary to boot application processors on multicore systems, since the real mode boot code
    // must be located somewhere in the lower memory area.
    pageTableMemory += createInitialMapping(*pageDirectory, pageTableMemory, 0x00000000, 0x00000000, 256);

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

    // Initialize Paging Area Manager -> Manages the virtual addresses of all page tables and directories
    auto usedPagingAreaPages = (reinterpret_cast<uint32_t>(pageTableMemory) - pagingAreaPhysical) / Kernel::Paging::PAGESIZE;
    auto *pagingAreaManager = new Kernel::PagingAreaManager(reinterpret_cast<uint8_t*>(pagingAreaVirtual), INITIAL_PAGING_AREA_SIZE / Kernel::Paging::PAGESIZE, usedPagingAreaPages);

    // Since page tables are not identity mapped, we need a separate page directory to keep track of the page table's virtual addresses.
    // This page directory is never accessed by hardware and is only used by the OS to access page tables.
    // Furthermore, we can no longer access the regular page directory using the current pointer and need to change it to its virtual equivalent.
    pageDirectory = reinterpret_cast<Kernel::Paging::Table*>(pagingAreaVirtual);
    auto *virtualPageDirectory = new Kernel::Paging::Table(*pageDirectory);

    // The virtual page directory starts as a 1:1-copy of the regular page directory.
    // We know that all page tables have been allocated consecutively from 'pageTableMemory', which is mapped to 'pagingAreaVirtual'.
    // The first page in 'pageTableMemory' is used for the page directory itself and page tables start afterward.
    uint32_t updatedEntries = 0;
    for (uint32_t i = 0; i < Kernel::Paging::ENTRIES_PER_TABLE; i++) {
        auto &entry = (*virtualPageDirectory)[i];
        if (!entry.isUnused()) {
            // Update entry with virtual page table address
            entry.set(reinterpret_cast<uint32_t>(pagingAreaVirtual + Kernel::Paging::PAGESIZE + (Kernel::Paging::PAGESIZE * updatedEntries++)), entry.getFlags());
        }
    }

    // Initialize page frame allocator -> Manages physical memory
    auto *pageFrameAllocator = new Kernel::PageFrameAllocator(*pagingAreaManager, nullptr, reinterpret_cast<uint8_t*>(physicalMemoryLimit - 1));
    // Reserve low memory (used by BIOS and ISA devices)
    pageFrameAllocator->setMemory(nullptr, reinterpret_cast<uint8_t*>(0x100000), 0, true);
    // Reserve kernel
    pageFrameAllocator->setMemory(reinterpret_cast<uint8_t*>(KERNEL_DATA_START), reinterpret_cast<uint8_t *>(KERNEL_DATA_START + kernelSize), 0, true);
    // Mark mapped kernel heap and paging area memory as used
    pageFrameAllocator->setMemory(reinterpret_cast<uint8_t*>(bootstrapMemory), reinterpret_cast<uint8_t*>(bootstrapMemory + INITIAL_PAGING_AREA_SIZE + INITIAL_KERNEL_HEAP_SIZE), 1, false);

    // Reserve corresponding blocks in memory map
    for (uint32_t i = 0; i < memoryMapEntries; i++) {
        auto currentAddress = reinterpret_cast<uint32_t>(memoryMap) + sizeof(Kernel::Multiboot::MemoryMapHeader) + i * memoryMap->entrySize;
        auto &entry = *reinterpret_cast<Kernel::Multiboot::MemoryMapEntry*>(currentAddress);

        if (entry.type == Kernel::Multiboot::RESERVED) {
            pageFrameAllocator->setMemory(reinterpret_cast<uint8_t*>(entry.address), reinterpret_cast<uint8_t*>(entry.address + entry.length), 0, true);
        }
    }

    // Initialize static data structures
    _init();

    // Create kernel address space and memory service
    auto *addressSpace = new Kernel::VirtualAddressSpace(pageDirectory, virtualPageDirectory, *kernelHeap);
    auto *memoryService = new Kernel::MemoryService(pageFrameAllocator, pagingAreaManager, addressSpace);

    // Create interrupt descriptor table
    auto *idt = new Kernel::InterruptDescriptorTable();
    idt->load();

    // The memory service and IDT are initialized and after registering the memory service, page faults can be handled,
    // which means, that we can fully use the kernel heap
    Kernel::Service::registerService(Kernel::MemoryService::SERVICE_ID, memoryService);
    memoryManagementInitialized = true;
    log.info("Welcome to hhuOS!");

    // Identity map lower memory (1 MiB), used for ISA DMA and BIOS related stuff
    // Depending on the system and bootloader, this may be necessary to initialize ACPI and SMBIOS data structures
    memoryService->mapPhysical(nullptr, nullptr, 256, Kernel::Paging::PRESENT | Kernel::Paging::WRITABLE);

    // Map Multiboot2 tags
    const auto multibootPageOffset = reinterpret_cast<uint32_t>(&multiboot) % Kernel::Paging::PAGESIZE;
    const auto multibootPages = multibootSize % Kernel::Paging::PAGESIZE == 0 ? multibootSize / Kernel::Paging::PAGESIZE : multibootSize / Kernel::Paging::PAGESIZE + 1;
    uint8_t *multibootVirtualAddress = static_cast<uint8_t*>(memoryService->mapIO(const_cast<void *>(reinterpret_cast<const void *>(&multiboot)), multibootPages, true)) + multibootPageOffset;

    // Initialize information service, which holds global system information like Multiboot, ACPI, etc.
    // It only has Multiboot information at the beginning, but other structures follow later in the boot process
    auto *informationService = new Kernel::InformationService(reinterpret_cast<Kernel::Multiboot*>(multibootVirtualAddress));
    Kernel::Service::registerService(Kernel::InformationService::SERVICE_ID, informationService);

    // Initialize ACPI and add it to the information service
    auto *acpi = new Device::Acpi();
    informationService->setAcpi(acpi);

    // Initialize SMBIOS and add it to the information service
    auto *smBios = new Device::SmBios();
    informationService->setSmBios(smBios);

    // Initialize the classic PIC and interrupt service
    auto *pic = new Device::Pic();
    auto *interruptService = new Kernel::InterruptService(pic);
    Kernel::Service::registerService(Kernel::InterruptService::SERVICE_ID, interruptService);

    // Switch to APIC, if available
    if (Device::Apic::isAvailable()) {
        log.info("APIC detected");
        auto *apic = Device::Apic::initialize();
        if (apic == nullptr) {
            log.warn("Failed to initialize APIC -> Falling back to PIC");
        } else {
            interruptService->useApic(apic);

            if (apic ->isSymmetricMultiprocessingSupported()) {
                apic->startupApplicationProcessors();
            }
        }
    } else {
        log.info("APIC not available -> Falling back to PIC");
    }

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Once you entered the gates of hell, you are not allowed to leave!");
}

Util::HeapMemoryManager& GatesOfHell::getKernelHeap() {
    return *kernelHeap;
}

bool GatesOfHell::isMemoryManagementInitialized() {
    return memoryManagementInitialized;
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
