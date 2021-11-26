/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <lib/util/memory/Address.h>
#include <asm_interface.h>
#include <device/port/serial/SerialPort.h>
#include <device/cpu/Cpu.h>
#include <device/time/Rtc.h>
#include <device/time/Pit.h>
#include <kernel/multiboot/Structure.h>
#include <kernel/paging/MemLayout.h>
#include <kernel/service/JobService.h>
#include <kernel/service/TimeService.h>
#include <kernel/memory/PagingAreaManagerRefillRunnable.h>
#include "System.h"

namespace Kernel {

bool System::initialized = false;
Util::Async::Spinlock System::serviceLock;
Util::Data::HashMap<Util::Memory::String, Service*> System::serviceMap(sizeof(uint8_t));
HeapMemoryManager *System::kernelHeapMemoryManager = nullptr;
MemoryService *System::memoryService = nullptr;
TaskStateSegment System::taskStateSegment{};
SystemCall System::systemCall{};
Logger System::log = Logger::get("System");

/**
 * Is called from assembly code before calling the main function, because it sets up
 * everything to get the system run.
 */
void System::initializeSystem(Multiboot::Info *multibootInfoAddress) {
    Multiboot::Structure::initialize(multibootInfoAddress);

    kernelHeapMemoryManager = &initializeKernelHeap();

    uint32_t physicalMemorySize = calculatePhysicalMemorySize();
    // We need at least 10 MiB physical memory to run properly
    if (physicalMemorySize < 10 * 1024 * 1024) {
        Util::Exception::throwException(Util::Exception::OUT_OF_PHYSICAL_MEMORY, "Not enough physical memory available to run hhuOS!");
    }

    // Initialize Paging Area Manager -> Manages the virtual addresses of all page tables and directories
    auto *pagingAreaManager = new PagingAreaManager();

    // Physical Page Frame Allocator is initialized to be possible to allocate physical memory (page frames)
    auto *pageFrameAllocator = new PageFrameAllocator(*pagingAreaManager, 0, physicalMemorySize - 1);

    // To be able to map new pages, a bootstrap address space is created.
    // It uses only the basePageDirectory with mapping for kernel space.
    auto *kernelAddressSpace = new VirtualAddressSpace(*kernelHeapMemoryManager);

    // Create memory service and register it to handle page faults
    memoryService = new MemoryService(pageFrameAllocator, pagingAreaManager, kernelAddressSpace);
    memoryService->plugin();

    kernelAddressSpace->initialize();
    memoryService->switchAddressSpace(*kernelAddressSpace);

    // Initialize global objects afterwards, because now missing pages can be mapped
    _init();

    // The base system is initialized. We can now enable interrupts and setup timer devices
    Device::Cpu::enableInterrupts();

    // Setup time and date devices
    auto *pit = new Device::Pit();
    pit->plugin();

    if (Device::Rtc::isAvailable()) {
        auto *rtc = new Device::Rtc();
        rtc->plugin();

        System::registerService(TimeService::SERVICE_NAME, new Kernel::TimeService(pit, rtc));
        System::registerService(JobService::SERVICE_NAME, new Kernel::JobService(*rtc, *pit));

        log.info("Base system initialized");
        log.info("RTC detected");
        if (!Device::Rtc::isValid()) {
            log.warn("CMOS has been cleared -> RTC is probably providing invalid date and time");
        }
    } else {
        System::registerService(TimeService::SERVICE_NAME, new Kernel::TimeService(pit, nullptr));
        System::registerService(JobService::SERVICE_NAME, new Kernel::JobService(*pit, *pit));
        log.info("Base system initialized");
        log.warn("RTC not detected -> Real time cannot be provided");
    }

    System::getService<JobService>().registerJob(*new PagingAreaManagerRefillRunnable(*pagingAreaManager), Job::Priority::HIGH, Util::Time::Timestamp(0, 1000000000));

    // Enable system calls
    log.info("Enabling system calls");
    systemCall.plugin();

    // Parse multiboot structure
    log.info("Parsing multiboot structure");
    Multiboot::Structure::parse();

    // Protect kernel code
    kernelAddressSpace->getPageDirectory().protectPages(___WRITE_PROTECTED_START__, ___WRITE_PROTECTED_END__);

    initialized = true;
}

void *System::allocateEarlyMemory(uint32_t size) {
    if (isInitialized()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "allocateEarlyMemory() called after system has been initialized!");
    }

    return kernelHeapMemoryManager->allocateMemory(size, 0);
}

void System::freeEarlyMemory(void *pointer) {
    if (isInitialized()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "freeEarlyMemory() called after system has been initialized!");
    }

    kernelHeapMemoryManager->freeMemory(pointer, 0);
}

void System::registerService(const Util::Memory::String &serviceId, Service *kernelService) {
    serviceLock.acquire();
    serviceMap.put(serviceId, kernelService);
    serviceLock.release();
}

bool System::isServiceRegistered(const Util::Memory::String &serviceId) {
    return serviceMap.containsKey(serviceId);
}

MemoryService &System::getMemoryService() {
    return *memoryService;
}

void System::panic(InterruptFrame *frame) {
    Device::Cpu::disableInterrupts();
    Device::Cpu::halt();
}

/**
 * Sets up the GDT for the system and a special GDT for BIOS-calls.
 * Only these two GDTs are needed, because memory protection and abstractions is done via paging.
 * The memory where the parameters point to is reserved in assembler code before paging is enabled.
 * Therefore we assume that the given pointers are physical addresses  - this is very important
 * to guarantee correct GDT descriptors using this setup-function.
 *
 * @param systemGdt Pointer to the GDT of the system
 * @param biosGdt Pointer to the GDT for BIOS-calls
 * @param systemGdtDescriptor Pointer to the descriptor of GDT; this descriptor should contain the virtual address of GDT
 * @param biosGdtDescriptor Pointer to the descriptor of BIOS-GDT; this descriptor should contain the physical address of BIOS-GDT
 * @param physicalGdtDescriptor Pointer to the descriptor of GDT; this descriptor should contain the physical address of GDT
 */
void System::initializeGlobalDescriptorTables(uint16_t *systemGdt, uint16_t *biosGdt, uint16_t *systemGdtDescriptor, uint16_t *biosGdtDescriptor, uint16_t *physicalGdtDescriptor) {
    // Set first 6 GDT entries to 0
    Util::Memory::Address<uint32_t>(systemGdt).setRange(0, 48);

    // Set first 4 bios GDT entries to 0
    Util::Memory::Address<uint32_t>(biosGdt).setRange(0, 32);

    // first set up general GDT for the system
    // first entry has to be null
    System::createGlobalDescriptorTableEntry(systemGdt, 0, 0, 0, 0, 0);
    // kernel code segment
    System::createGlobalDescriptorTableEntry(systemGdt, 1, 0, 0xFFFFFFFF, 0x9A, 0xC);
    // kernel data segment
    System::createGlobalDescriptorTableEntry(systemGdt, 2, 0, 0xFFFFFFFF, 0x92, 0xC);
    // user code segment
    System::createGlobalDescriptorTableEntry(systemGdt, 3, 0, 0xFFFFFFFF, 0xFA, 0xC);
    // user data segment
    System::createGlobalDescriptorTableEntry(systemGdt, 4, 0, 0xFFFFFFFF, 0xF2, 0xC);
    // tss segment
    System::createGlobalDescriptorTableEntry(systemGdt, 5,
                                                 reinterpret_cast<uint32_t>(&System::getTaskStateSegment()),
                                                 sizeof(Kernel::TaskStateSegment), 0x89, 0x4);

    // set up descriptor for GDT
    *((uint16_t *) systemGdtDescriptor) = 6 * 8;
    // the normal descriptor should contain the virtual address of GDT
    *((uint32_t *) (systemGdtDescriptor + 1)) = (uint32_t) systemGdt + Kernel::MemoryLayout::VIRT_KERNEL_START;

    // set up descriptor for GDT with phys. address - needed for bootstrapping
    *((uint16_t *) physicalGdtDescriptor) = 6 * 8;
    // this descriptor should contain the physical address of GDT
    *((uint32_t *) (physicalGdtDescriptor + 1)) = (uint32_t) systemGdt;

    // now set up GDT for BIOS-calls (notice that no userspace entries are necessary here)
    // first entry has to be null
    System::createGlobalDescriptorTableEntry(biosGdt, 0, 0, 0, 0, 0);
    // kernel code segment
    System::createGlobalDescriptorTableEntry(biosGdt, 1, 0, 0xFFFFFFFF, 0x9A, 0xC);
    // kernel data segment
    System::createGlobalDescriptorTableEntry(biosGdt, 2, 0, 0xFFFFFFFF, 0x92, 0xC);
    // prepared BIOS-call segment (contains 16-bit code etc...)
    System::createGlobalDescriptorTableEntry(biosGdt, 3, 0x4000, 0xFFFFFFFF, 0x9A, 0x8);


    // set up descriptor for BIOS-GDT
    *((uint16_t *) biosGdtDescriptor) = 4 * 8;
    // the descriptor should contain physical address of BIOS-GDT because paging is not enabled during BIOS-calls
    *((uint32_t *) (biosGdtDescriptor + 1)) = (uint32_t) biosGdt;
}

/**
 * Creates an entry into a given GDT (Global Descriptor Table).
 * Memory for the GDT must be allocated before.
 */
void System::createGlobalDescriptorTableEntry(uint16_t *gdt, uint16_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    // each GDT-entry consists of 4 16-bit unsigned integers
    // calculate index into 16bit-array that represents GDT
    uint16_t idx = 4 * num;

    // first 16-bit value: [Limit 0:15]
    gdt[idx] = (uint16_t) (limit & 0xFFFF);
    // second 16-bit value: [Base 0:15]
    gdt[idx + 1] = (uint16_t) (base & 0xFFFF);
    // third 16-bit value: [Access Byte][Base 16:23]
    gdt[idx + 2] = (uint16_t) ((base >> 16) & 0xFF) | (access << 8);
    // fourth 16-bit value: [Base 24:31][Flags][Limit 16:19]
    gdt[idx + 3] = (uint16_t) ((limit >> 16) & 0x0F) | ((flags << 4) & 0xF0) | ((base >> 16) & 0xFF00);
    // end of GDT-entry
}

/**
 * Checks if the system management is fully initialized.
 */
bool System::isInitialized() {
    return initialized;
}

uint32_t System::calculatePhysicalMemorySize() {
    Util::Data::Array<Multiboot::MemoryMapEntry> memoryMap = Multiboot::Structure::getMemoryMap();
    Multiboot::MemoryMapEntry &maxEntry = memoryMap[0];
    for (const auto &entry : memoryMap) {
        if (entry.type != Multiboot::MULTIBOOT_MEMORY_AVAILABLE) {
            continue;
        }

        if (entry.address + entry.length > maxEntry.address + maxEntry.length) {
            maxEntry = entry;
        }
    }

    if (maxEntry.type != Multiboot::MULTIBOOT_MEMORY_AVAILABLE) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "No usable memory found!");
    }

    return static_cast<uint32_t>(maxEntry.address + maxEntry.length);
}

HeapMemoryManager& System::initializeKernelHeap() {
    auto *blockMap = Multiboot::Structure::getBlockMap();

    for (uint32_t i = 0; blockMap[i].blockCount != 0; i++) {
        const auto &block = blockMap[i];

        if (block.type == Multiboot::Structure::HEAP_RESERVED) {
            static FreeListMemoryManager heapMemoryManager;
            heapMemoryManager.initialize(block.virtualStartAddress, Kernel::MemoryLayout::VIRT_KERNEL_HEAP_END);
            return heapMemoryManager;
        }
    }

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "No 4 MiB block available for bootstrapping the kernel heap memory manager!");
}

TaskStateSegment &System::getTaskStateSegment() {
    return taskStateSegment;
}

}