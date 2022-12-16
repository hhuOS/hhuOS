/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/memory/Address.h"
#include "asm_interface.h"
#include "device/cpu/Cpu.h"
#include "device/time/Rtc.h"
#include "device/time/Pit.h"
#include "kernel/paging/MemoryLayout.h"
#include "kernel/service/TimeService.h"
#include "kernel/memory/PagingAreaManagerRefillRunnable.h"
#include "kernel/paging/Paging.h"
#include "System.h"
#include "lib/util/reflection/InstanceFactory.h"
#include "kernel/service/StorageService.h"
#include "kernel/service/InterruptService.h"
#include "kernel/service/ProcessService.h"
#include "BlueScreen.h"
#include "device/power/acpi/Acpi.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/interrupt/InterruptHandler.h"
#include "kernel/log/Logger.h"
#include "kernel/memory/PageFrameAllocator.h"
#include "kernel/memory/PagingAreaManager.h"
#include "kernel/multiboot/Multiboot.h"
#include "kernel/paging/PageDirectory.h"
#include "kernel/paging/VirtualAddressSpace.h"
#include "kernel/process/Thread.h"
#include "kernel/process/ThreadState.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/SchedulerService.h"
#include "kernel/system/SystemCall.h"
#include "kernel/system/TaskStateSegment.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/data/Array.h"
#include "lib/util/memory/FreeListMemoryManager.h"
#include "lib/util/memory/HeapMemoryManager.h"

namespace Kernel {
class Service;

bool System::initialized = false;
Util::Async::Spinlock System::serviceLock;
Service* System::serviceMap[256]{};
Util::Memory::HeapMemoryManager *System::kernelHeapMemoryManager{};
InterruptHandler *System::pagefaultHandler{};
TaskStateSegment System::taskStateSegment{};
SystemCall System::systemCall{};
Logger System::log = Logger::get("System");

/**
 * Is called from assembly code before calling the main function, because it sets up
 * everything to get the system run.
 */
void System::initializeSystem() {
    Multiboot::initialize();
    Device::Acpi::initialize();

    kernelHeapMemoryManager = &initializeKernelHeap();

    uint32_t physicalMemorySize = calculatePhysicalMemorySize();

    // Initialize Paging Area Manager -> Manages the virtual addresses of all page tables and directories
    auto *pagingAreaManager = new PagingAreaManager();

    // Physical Page Frame Allocator is initialized to be possible to allocate physical memory (page frames)
    auto *pageFrameAllocator = new PageFrameAllocator(*pagingAreaManager, 0, physicalMemorySize - 1);

    // To be able to map new pages, a bootstrap address space is created.
    // It uses only the basePageDirectory with mapping for kernel space.
    auto *kernelAddressSpace = new VirtualAddressSpace(*kernelHeapMemoryManager);

    // Create memory and interrupt services, so that the memory service can handle page faults
    auto *memoryService = new MemoryService(pageFrameAllocator, pagingAreaManager, kernelAddressSpace);
    memoryService->switchAddressSpace(*kernelAddressSpace);
    pagefaultHandler = memoryService;

    // Initialize global objects afterwards, because now missing pages can be mapped
    _init();

    // Register services after _init(), since the static objects serviceMap and serviceLock have now been initialized
    auto *interruptService = new InterruptService();
    registerService(InterruptService::SERVICE_ID, interruptService);
    registerService(MemoryService::SERVICE_ID, memoryService);
    memoryService->plugin();
    log.info("Welcome to hhuOS!");
    log.info("Memory management has been initialized");

    // Create scheduler service and register kernel process
    log.info("Initializing scheduler");
    auto *schedulerService = new SchedulerService();
    auto *processService = new ProcessService();
    registerService(SchedulerService::SERVICE_ID, schedulerService);
    registerService(ProcessService::SERVICE_ID, processService);

    initialized = true;

    // The base system is initialized. We can now enable interrupts and initialize timer devices
    log.info("Enabling interrupts");
    Device::Cpu::enableInterrupts();

    // Setup time and date devices
    log.info("Initializing PIT");
    auto *pit = new Device::Pit();
    pit->plugin();

    Device::Rtc *rtc = nullptr;
    if (Device::Rtc::isAvailable()) {
        log.info("Initializing RTC");
        rtc = new Device::Rtc();
        rtc->plugin();

        if (!Device::Rtc::isValid()) {
            log.warn("CMOS has been cleared -> RTC is probably providing invalid date and time");
        }
    } else {
        log.warn("RTC not available");
    }

    registerService(TimeService::SERVICE_ID, new Kernel::TimeService(pit, rtc));

    // Create thread to refill block pool of paging area manager
    auto &refillThread = Kernel::Thread::createKernelThread("Paging-Area-Pool-Refiller", processService->getKernelProcess(), new PagingAreaManagerRefillRunnable(*pagingAreaManager));
    schedulerService->ready(refillThread);

    // Register memory manager
    Util::Reflection::InstanceFactory::registerPrototype(new Util::Memory::FreeListMemoryManager());

    // Register storage service
    registerService(StorageService::SERVICE_ID, new StorageService());

    // Enable system calls
    log.info("Enabling system calls");
    systemCall.plugin();

    // Protect kernel code
    kernelAddressSpace->getPageDirectory().unsetPageFlags(___WRITE_PROTECTED_START__, ___WRITE_PROTECTED_END__, Paging::READ_WRITE);
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

void System::registerService(uint32_t serviceId, Service *kernelService) {
    serviceLock.acquire();
    if (isServiceRegistered(serviceId)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Service is already registered!");
    }

    serviceMap[serviceId] = kernelService;
    serviceLock.release();
}

bool System::isServiceRegistered(uint32_t serviceId) {
    return serviceMap[serviceId] != nullptr;
}

void System::panic(const InterruptFrame &frame) {
    Device::Cpu::disableInterrupts();
    BlueScreen::show(frame);
    Device::Cpu::halt();
}

/**
 * Sets up the GDT for the system and a special GDT for BIOS-calls.
 * Only these two GDTs are needed, because memory protection and abstractions is done via paging.
 * The memory where the parameters point to is reserved in assembler code before paging is enabled.
 * Therefore we assume that the given pointers are physical addresses - this is very important
 * to guarantee correct GDT descriptors using this initialize function.
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
    System::createGlobalDescriptorTableEntry(systemGdt, 5, reinterpret_cast<uint32_t>(&System::taskStateSegment), sizeof(Kernel::TaskStateSegment), 0x89, 0x4);

    // set up descriptor for GDT
    *((uint16_t *) systemGdtDescriptor) = 6 * 8;
    // the normal descriptor should contain the virtual address of GDT
    *((uint32_t *) (systemGdtDescriptor + 1)) = (uint32_t) systemGdt + Kernel::MemoryLayout::KERNEL_START;

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
    System::createGlobalDescriptorTableEntry(biosGdt, 3, MemoryLayout::BIOS_CODE_MEMORY.startAddress, 0xFFFFFFFF, 0x9A, 0x8);


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
    Util::Data::Array<Multiboot::MemoryMapEntry> memoryMap = Multiboot::getMemoryMap();
    Multiboot::MemoryMapEntry &maxEntry = memoryMap[0];
    for (const auto &entry : memoryMap) {
        if (entry.type != Multiboot::AVAILABLE) {
            continue;
        }

        if (entry.address + entry.length > maxEntry.address + maxEntry.length) {
            maxEntry = entry;
        }
    }

    if (maxEntry.type != Multiboot::AVAILABLE) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "No usable memory found!");
    }

    return static_cast<uint32_t>(maxEntry.address + maxEntry.length);
}

Util::Memory::HeapMemoryManager& System::initializeKernelHeap() {
    auto *blockMap = Multiboot::getBlockMap();

    for (uint32_t i = 0; blockMap[i].blockCount != 0; i++) {
        const auto &block = blockMap[i];

        if (block.type == Multiboot::HEAP_RESERVED) {
            static Util::Memory::FreeListMemoryManager heapMemoryManager;
            heapMemoryManager.initialize(block.virtualStartAddress, Kernel::MemoryLayout::KERNEL_HEAP_END_ADDRESS);
            return heapMemoryManager;
        }
    }

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "No 4 MiB block available for bootstrapping the kernel heap memory manager!");
}

TaskStateSegment &System::getTaskStateSegment() {
    return taskStateSegment;
}

void System::handleEarlyInterrupt(const InterruptFrame &frame) {
    if (frame.interrupt == InterruptDispatcher::PAGEFAULT) {
        pagefaultHandler->trigger(frame);
    }
}

}