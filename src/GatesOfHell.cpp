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
#include "kernel/log/Log.h"
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
#include "kernel/log/Log.h"
#include "device/time/pit/Pit.h"
#include "device/time/rtc/Rtc.h"
#include "kernel/service/TimeService.h"
#include "kernel/service/SchedulerService.h"
#include "kernel/service/ProcessService.h"
#include "lib/util/hardware/CpuId.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/Terminal.h"
#include "lib/util/graphic/LinearFrameBufferTerminal.h"
#include "device/graphic/ColorGraphicsAdapter.h"
#include "device/bus/pci/Pci.h"
#include "kernel/service/StorageService.h"
#include "device/storage/virtual/VirtualDiskDrive.h"
#include "device/storage/ide/IdeController.h"
#include "device/storage/floppy/FloppyController.h"
#include "kernel/service/FilesystemService.h"
#include "lib/util/reflection/InstanceFactory.h"
#include "filesystem/fat/FatDriver.h"
#include "filesystem/memory/MemoryDriver.h"
#include "filesystem/process/ProcessDriver.h"
#include "filesystem/memory/NullNode.h"
#include "filesystem/memory/ZeroNode.h"
#include "filesystem/memory/RandomNode.h"
#include "filesystem/memory/MountsNode.h"
#include "kernel/memory/MemoryStatusNode.h"
#include "device/system/FirmwareConfiguration.h"
#include "filesystem/qemu/FirmwareConfigurationDriver.h"
#include "filesystem/acpi/AcpiDriver.h"
#include "filesystem/smbios/SmBiosDriver.h"
#include "device/graphic/LinearFrameBufferNode.h"
#include "device/graphic/TerminalNode.h"
#include "device/port/parallel/ParallelPort.h"
#include "lib/util/io/stream/FileOutputStream.h"
#include "device/hid/Mouse.h"
#include "device/hid/Keyboard.h"
#include "kernel/service/NetworkService.h"
#include "device/network/rtl8139/Rtl8139.h"
#include "device/sound/speaker/PcSpeakerNode.h"
#include "device/sound/soundblaster/SoundBlaster.h"
#include "kernel/service/PowerManagementService.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/System.h"
#include "BuildConfig.h"
#include "kernel/memory/PagingAreaManagerRefillRunnable.h"
#include "lib/util/async/Process.h"

extern const uint32_t ___KERNEL_DATA_START__;
extern const uint32_t ___KERNEL_DATA_END__;
extern const uint32_t ___WRITE_PROTECTED_START__;
extern const uint32_t ___WRITE_PROTECTED_END__;

const uint32_t KERNEL_DATA_START = reinterpret_cast<uint32_t>(&___KERNEL_DATA_START__);
const uint32_t KERNEL_DATA_END = reinterpret_cast<uint32_t>(&___KERNEL_DATA_END__);
const uint32_t WRITE_PROTECTED_START = reinterpret_cast<uint32_t>(&___WRITE_PROTECTED_START__);
const uint32_t WRITE_PROTECTED_END = reinterpret_cast<uint32_t>(&___WRITE_PROTECTED_END__);

const constexpr uint32_t INITIAL_PAGING_AREA_SIZE = 256 * 1024;
const constexpr uint32_t INITIAL_KERNEL_HEAP_SIZE = 32 * 1024;

extern "C" {
    void _init();
}

extern "C" int32_t atexit (void (*func)()) noexcept {
    return 0;
}

namespace Device {
class Machine;
}  // namespace Device

Util::HeapMemoryManager *GatesOfHell::kernelHeap = nullptr;

extern "C" void start(uint32_t multibootMagic, const Kernel::Multiboot *multiboot) {
    GatesOfHell::enter(multibootMagic, multiboot);
}

void GatesOfHell::enter(uint32_t multibootMagic, const Kernel::Multiboot *multiboot) {
    // Check multiboot magic number
    LOG_INFO("Welcome to hhuOS early boot environment!");
    if (multibootMagic != Kernel::Multiboot::MAGIC) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Multiboot2 magic number is invalid!");
    }

    // Scan memory map
    LOG_INFO("Scanning memory map to calculate physical address limit");
    auto multibootAddress = reinterpret_cast<uint32_t>(multiboot);
    auto multibootSize = multiboot->getSize();
    auto *memoryMap = &(multiboot->getTag<Kernel::Multiboot::MemoryMapHeader>(Kernel::Multiboot::MEMORY_MAP));
    auto memoryMapEntries = (memoryMap->tagHeader.size - sizeof(Kernel::Multiboot::TagHeader)) / memoryMap->entrySize;

    uint32_t physicalMemoryLimit = 0;
    for (uint32_t i = 0; i < memoryMapEntries; i++) {
        auto currentAddress = reinterpret_cast<uint32_t>(memoryMap) + sizeof(Kernel::Multiboot::MemoryMapHeader) + i * memoryMap->entrySize;
        auto &entry = *reinterpret_cast<Kernel::Multiboot::MemoryMapEntry*>(currentAddress);

        if (entry.type == Kernel::Multiboot::AVAILABLE && (entry.address + entry.length) > physicalMemoryLimit) {
            physicalMemoryLimit = entry.address + entry.length;
        }
    }

    // Use lower memory (< 1 MiB) below kernel to bootstrap paging and kernel heap
    uint32_t bootstrapMemory = Kernel::MemoryLayout::USABLE_LOWER_MEMORY.startAddress;

    // Bootloaders may also place the multiboot information inside this area (e.g. Limine), so we need to check for that
    if (multibootAddress < Kernel::MemoryLayout::USABLE_LOWER_MEMORY.endAddress) {
        // Check free space before multiboot info
        if (multibootAddress - Kernel::MemoryLayout::USABLE_LOWER_MEMORY.startAddress < INITIAL_PAGING_AREA_SIZE + INITIAL_KERNEL_HEAP_SIZE) {
            // Not enough space before multiboot info -> Use memory after multiboot info
            bootstrapMemory = Util::Address<uint32_t>(multibootAddress + multibootSize).alignUp(Kernel::Paging::PAGESIZE).get();
        }
    }

    uint32_t pagingAreaPhysical = bootstrapMemory;
    uint32_t kernelHeapPhysical = bootstrapMemory + INITIAL_PAGING_AREA_SIZE;

    LOG_INFO("Creating initial mappings");

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
    LOG_INFO("Enabling paging");
    Kernel::Paging::loadDirectory(*pageDirectory);
    Device::Cpu::writeCr0(Device::Cpu::readCr0() | Device::Cpu::PAGING);

    // Initialize kernel heap
    LOG_INFO("Initializing kernel heap");
    static Util::FreeListMemoryManager kernelHeapManager;
    kernelHeapManager.initialize(reinterpret_cast<uint8_t*>(kernelHeapVirtual), reinterpret_cast<uint8_t*>(Kernel::MemoryLayout::KERNEL_HEAP_END_ADDRESS));
    kernelHeap = &kernelHeapManager;
    LOG_INFO("Kernel heap initialized (Bootstrap memory: [0x%08x])", bootstrapMemory);

    // Setup GDT
    LOG_INFO("Setting up global descriptor table");
    auto *tss = new Kernel::GlobalDescriptorTable::TaskStateSegment();
    auto *gdt = new Kernel::GlobalDescriptorTable();
    gdt->addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x9a, 0x0c)); // Kernel code segment
    gdt->addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x92, 0x0c)); // Kernel data segment
    gdt->addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xfa, 0x0c)); // User code segment
    gdt->addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0xf2, 0x0c)); // User data segment
    gdt->addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(reinterpret_cast<uint32_t>(tss), sizeof(Kernel::GlobalDescriptorTable::TaskStateSegment), 0x89, 0x04));
    gdt->load();

    // Load task state segment
    Device::Cpu::loadTaskStateSegment(Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 5));

    // Set segment registers
    Device::Cpu::setSegmentRegister(Device::Cpu::CS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 1));
    Device::Cpu::setSegmentRegister(Device::Cpu::SS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::DS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::ES, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::FS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));
    Device::Cpu::setSegmentRegister(Device::Cpu::GS, Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2));

    // Initialize Paging Area Manager -> Manages the virtual addresses of all page tables and directories
    LOG_INFO("Initializing paging area manager");
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
    LOG_INFO("Initializing page frame allocator");
    auto *pageFrameAllocator = new Kernel::PageFrameAllocator(*pagingAreaManager, nullptr, reinterpret_cast<uint8_t*>(physicalMemoryLimit - 1));
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
    LOG_INFO("Initializing static data structures");
    _init();

    // Create kernel address space and memory service
    LOG_INFO("Initializing kernel address space");
    auto *kernelAddressSpace = new Kernel::VirtualAddressSpace(pageDirectory, virtualPageDirectory, *kernelHeap);
    auto *memoryService = new Kernel::MemoryService(gdt, tss, pageFrameAllocator, pagingAreaManager, kernelAddressSpace);

    // Create interrupt descriptor table
    LOG_INFO("Initializing interrupt descriptor table");
    auto *idt = new Kernel::InterruptDescriptorTable();
    idt->load();

    // The memory service and IDT are initialized and after registering the memory service, page faults can be handled,
    // which means, that we can fully use the kernel heap
    Kernel::Service::registerService(Kernel::MemoryService::SERVICE_ID, memoryService);
    LOG_INFO("Welcome to hhuOS!");
    LOG_INFO("Used kernel heap memory during early boot process: [%u Byte]", kernelHeapManager.getTotalMemory() - kernelHeapManager.getFreeMemory());

    LOG_INFO("Creating remaining mappings, needed for booting");
    // Identity map BIOS related parts of the lower 1 MiB
    // Depending on the system and bootloader, this may be necessary to initialize ACPI and SMBIOS data structures
    // The startup code for application processors is also located there
    memoryService->mapPhysical(nullptr, nullptr, Kernel::MemoryLayout::USABLE_LOWER_MEMORY.startAddress / Kernel::Paging::PAGESIZE, Kernel::Paging::PRESENT | Kernel::Paging::WRITABLE);
    memoryService->mapPhysical(reinterpret_cast<void*>(Kernel::MemoryLayout::USABLE_LOWER_MEMORY.endAddress + 1), reinterpret_cast<void*>(Kernel::MemoryLayout::USABLE_LOWER_MEMORY.endAddress + 1),
                               (KERNEL_DATA_START - (Kernel::MemoryLayout::USABLE_LOWER_MEMORY.endAddress + 1)) / Kernel::Paging::PAGESIZE, Kernel::Paging::PRESENT | Kernel::Paging::WRITABLE);

    // Detect CPU features
    if (Util::Hardware::CpuId::isAvailable()) {
        const auto info = Util::Hardware::CpuId::getCpuInfo();
        LOG_INFO("CPU info: Vendor [%s], Family [%u], Model [%u], Stepping [%u], Type [%u]", static_cast<const char*>(Util::Hardware::CpuId::getVendorString()), info.family, info.model, info.stepping, info.type);

        const auto features = Util::Hardware::CpuId::getCpuFeatures();
        Util::String featureString;
        for (uint32_t i = 0; i < features.length(); i++) {
            featureString += Util::Hardware::CpuId::getFeatureAsString(features[i]) + Util::String(" ");
        }
        LOG_INFO("CPU features: %s", static_cast<const char*>(featureString));
    } else {
        LOG_ERROR("CPUID not available!");
    }

    // Map Multiboot2 tags
    const auto multibootPageOffset = reinterpret_cast<uint32_t>(multiboot) % Kernel::Paging::PAGESIZE;
    const auto multibootPages = multibootSize % Kernel::Paging::PAGESIZE == 0 ? multibootSize / Kernel::Paging::PAGESIZE : multibootSize / Kernel::Paging::PAGESIZE + 1;
    const auto *multibootVirtualAddress = static_cast<const uint8_t*>(memoryService->mapIO(const_cast<void*>(reinterpret_cast<const void*>(multiboot)), multibootPages, true)) + multibootPageOffset;
    multiboot = reinterpret_cast<const Kernel::Multiboot*>(multibootVirtualAddress);
    LOG_INFO("Bootloader: [%s], Multiboot size: [%u Byte]", static_cast<const char*>(multiboot->getBootloaderName()), multiboot->getSize());

    // Set log level
    if (multiboot->hasKernelOption("log_level")) {
        const auto level = multiboot->getKernelOption("log_level");
        Kernel::Log::setLevel(level);
    }

    const auto tagTypes = multiboot->getAvailableTagTypes();
    Util::String tagString;
    for (uint32_t i = 0; i < tagTypes.length(); i++) {
        tagString += Util::String::format("%u", tagTypes[i]) + Util::String(" ");
    }
    LOG_INFO("Multiboot tags: %s", static_cast<const char*>(tagString));

    // Initialize information service, which holds global system information like Multiboot, ACPI, etc.
    // It only has Multiboot information at the beginning, but other structures follow later in the boot process
    auto *informationService = new Kernel::InformationService(multiboot);
    Kernel::Service::registerService(Kernel::InformationService::SERVICE_ID, informationService);

    // Initialize ACPI and add it to the information service
    LOG_INFO("Initializing ACPI");
    auto *acpi = new Device::Acpi();
    informationService->setAcpi(acpi);

    // Initialize SMBIOS and add it to the information
    LOG_INFO("Initializing SMBIOS");
    auto *smBios = new Device::SmBios();
    informationService->setSmBios(smBios);
    auto &biosInformation = smBios->getTable<Util::Hardware::SmBios::BiosInformation>(Util::Hardware::SmBios::BIOS_INFORMATION);
    LOG_INFO("BIOS vendor: [%s], BIOS version: [%s (%s)]", biosInformation.getVendorName(), biosInformation.getVersion(), biosInformation.getReleaseDate());

    // Initialize the classic PIC and interrupt service
    LOG_INFO("Initializing classic PIC");
    auto *pic = new Device::Pic();
    auto *interruptService = new Kernel::InterruptService(pic);
    Kernel::Service::registerService(Kernel::InterruptService::SERVICE_ID, interruptService);

    // Switch to APIC, if available
    if (Device::Apic::isAvailable()) {
        LOG_INFO("APIC detected");
        auto *apic = Device::Apic::initialize();
        if (apic == nullptr) {
            LOG_WARN("Failed to initialize APIC -> Falling back to PIC");
        } else {
            interruptService->useApic(apic);

            if (apic ->isSymmetricMultiprocessingSupported()) {
                apic->startupApplicationProcessors();
            }
        }
    } else {
        LOG_INFO("APIC not available -> Falling back to PIC");
    }

    // Create scheduler and process service and register kernel process;
    LOG_INFO("Initializing scheduler");
    auto *kernelProcess = new Kernel::Process(*kernelAddressSpace, "Kernel");
    auto *schedulerService = new Kernel::SchedulerService();
    auto *processService = new Kernel::ProcessService(kernelProcess);
    Kernel::Service::registerService(Kernel::SchedulerService::SERVICE_ID, schedulerService);
    Kernel::Service::registerService(Kernel::ProcessService::SERVICE_ID, processService);

    // Create thread to refill block pool of paging area manager
    auto &refillThread = Kernel::Thread::createKernelThread("Paging-Area-Pool-Refiller", processService->getKernelProcess(), new Kernel::PagingAreaManagerRefillRunnable(*pagingAreaManager));
    schedulerService->ready(refillThread);

    // Register memory manager
    Util::Reflection::InstanceFactory::registerPrototype(new Util::FreeListMemoryManager());

    // Protect kernel code
    for (uint32_t address = WRITE_PROTECTED_START; address < WRITE_PROTECTED_END; address += Kernel::Paging::PAGESIZE) {
        // Get indices into page table and directory
        uint32_t pageDirectoryIndex = Kernel::Paging::DIRECTORY_INDEX(reinterpret_cast<uint32_t>(address));
        uint32_t pageTableIndex = Kernel::Paging::TABLE_INDEX(reinterpret_cast<uint32_t>(address));

        // Disable READ/WRITE bit for page
        auto &pageTable = *reinterpret_cast<Kernel::Paging::Table*>((*virtualPageDirectory)[pageDirectoryIndex].getAddress());
        auto &entry = pageTable[pageTableIndex];
        entry.set(entry.getAddress(), entry.getFlags() & (~Kernel::Paging::WRITABLE));
    }

    // The base system is initialized -> We can now enable interrupts and initialize timer devices
    LOG_INFO("Enabling interrupts");
    Device::Cpu::enableInterrupts();

    // Initialize frame buffer
    Util::Graphic::LinearFrameBuffer *lfb = nullptr;
    Util::Graphic::Terminal *terminal;
    if (multiboot->hasTag(Kernel::Multiboot::FRAMEBUFFER_INFO)) {
        const auto &tag = multiboot->getTag<Kernel::Multiboot::FramebufferInfo>(Kernel::Multiboot::FRAMEBUFFER_INFO);

        if (tag.type == Kernel::Multiboot::EGA_TEXT) {
            LOG_INFO("Framebuffer info provided by bootloader (Type: [EGA_TEXT], Resolution: [%ux%u@%u], Address: [0x%08x])", tag.width, tag.height, tag.bpp, tag.address);

            if ((tag.width != 80 && tag.width != 40) && tag.height != 25) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Unsupported text mode resolution!");
            }

            terminal = new Device::Graphic::ColorGraphicsAdapter(tag.width, tag.height);
        } else if (tag.type == Kernel::Multiboot::RGB) {
            LOG_INFO("Framebuffer info provided by bootloader (Type: [RGB], Resolution: [%ux%u@%u], Address: [0x%08x])", tag.width, tag.height, tag.bpp, tag.address);

            lfb = new Util::Graphic::LinearFrameBuffer(tag.address, tag.width, tag.height, tag.bpp, tag.pitch, false);
            terminal = new Util::Graphic::LinearFrameBufferTerminal(lfb);
        } else {
            Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Unsupported frame buffer type!");
        }
    } else {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "No framebuffer information provided by bootloader!");
    }

    Kernel::Log::addOutputStream(*terminal);

    // Setup time and date devices
    LOG_INFO("Initializing PIT");
    auto *pit = new Device::Pit(1, 10);
    pit->plugin();

    Device::Rtc *rtc = nullptr;
    if (Device::Rtc::isAvailable()) {
        LOG_INFO("Initializing RTC");
        rtc = new Device::Rtc(250);
        rtc->plugin();

        if (!Device::Rtc::isValid()) {
            LOG_WARN("CMOS has been cleared -> RTC is probably providing invalid date and time");
        }
    } else {
        LOG_INFO("RTC not available");
    }

    Kernel::Service::registerService(Kernel::TimeService::SERVICE_ID, new Kernel::TimeService(pit, rtc));

    // Scan PCI bus
    Device::Pci::scan();

    // Initialize storage devices
    auto *storageService = new Kernel::StorageService();
    Kernel::Service::registerService(Kernel::StorageService::SERVICE_ID, storageService);

    LOG_INFO("Searching multiboot modules for virtual disk drive");
    if (multiboot->getModuleNames().contains("vdd0")) {
        auto &module = multiboot->getModule("vdd0");
        const auto sectorCount = (module.endAddress - module.startAddress) / 512;
        const auto modulePageOffset = module.startAddress % Kernel::Paging::PAGESIZE;
        const auto moduleSize = module.endAddress - module.startAddress;
        const auto modulePageCount = moduleSize % Kernel::Paging::PAGESIZE == 0 ? (moduleSize / Kernel::Paging::PAGESIZE) : (moduleSize / Kernel::Paging::PAGESIZE) + 1;

        auto *moduleVirtualAddress = memoryService->mapIO(reinterpret_cast<void*>(module.startAddress), modulePageCount);
        auto *device = new Device::Storage::VirtualDiskDrive(static_cast<uint8_t*>(moduleVirtualAddress) + modulePageOffset, 512, sectorCount);

        storageService->registerDevice(device, "vdd");
    }

    Device::Storage::IdeController::initializeAvailableControllers();

    if (Device::Storage::FloppyController::isAvailable()) {
        auto *floppyController = new Device::Storage::FloppyController();
        floppyController->initializeAvailableDrives();
    }

    // Initialize filesystem
    LOG_INFO("Initializing filesystem");
    auto *filesystemService = new Kernel::FilesystemService();
    Kernel::Service::registerService(Kernel::FilesystemService::SERVICE_ID, filesystemService);

    Util::Reflection::InstanceFactory::registerPrototype(new Filesystem::Fat::FatDriver());

    bool rootMounted = false;
    if (multiboot->hasKernelOption("root")) {
        auto rootOptions = multiboot->getKernelOption("root").split(",");
        if (rootOptions.length() >= 2) {
            const auto &deviceName = rootOptions[0];
            const auto &driverName = rootOptions[1];

            if (storageService->isDeviceRegistered(deviceName)) {
                LOG_INFO("Mounting [%s] to root using driver [%s]", static_cast<const char*>(deviceName), static_cast<const char*>(driverName));
                rootMounted = filesystemService->mount(deviceName, "/", driverName);
                if (!rootMounted) {
                    LOG_ERROR("Failed to mount root filesystem");
                }
            } else {
                LOG_ERROR("Device [%s] is not available", static_cast<const char*>(deviceName));
            }
        } else {
            LOG_ERROR("Invalid options for root filesystem given");
        }
    }

    if (!rootMounted) {
        LOG_INFO("Mounting virtual filesystem as root filesystem");
        auto *rootDriver = new Filesystem::Memory::MemoryDriver();
        filesystemService->getFilesystem().mountVirtualDriver("/", rootDriver);
    }

    auto *deviceDriver = new Filesystem::Memory::MemoryDriver();
    filesystemService->createDirectory("/device");
    filesystemService->getFilesystem().mountVirtualDriver("/device", deviceDriver);

    auto *processDriver = new Filesystem::Process::ProcessDriver();
    filesystemService->createDirectory("/process");
    filesystemService->getFilesystem().mountVirtualDriver("/process", processDriver);

    filesystemService->createFile("/device/log");
    deviceDriver->addNode("/", new Filesystem::Memory::NullNode());
    deviceDriver->addNode("/", new Filesystem::Memory::ZeroNode());
    deviceDriver->addNode("/", new Filesystem::Memory::RandomNode());
    deviceDriver->addNode("/", new Filesystem::Memory::MountsNode());
    deviceDriver->addNode("/", new Kernel::MemoryStatusNode("memory"));

    if (Device::FirmwareConfiguration::isAvailable()) {
        auto *fwCfg = new Device::FirmwareConfiguration();
        auto *qemuDriver = new Filesystem::Qemu::FirmwareConfigurationDriver(*fwCfg);
        filesystemService->createDirectory("/device/qemu");
        filesystemService->getFilesystem().mountVirtualDriver("/device/qemu", qemuDriver);
    }

    if (acpi->getAvailableTables().length() != 0) {
        auto *acpiDriver = new Filesystem::Acpi::AcpiDriver();
        filesystemService->createDirectory("/device/acpi");
        filesystemService->getFilesystem().mountVirtualDriver("/device/acpi", acpiDriver);
    }

    if (smBios->getAvailableTables().length() != 0) {
        auto *smBiosDriver = new Filesystem::SmBios::SmBiosDriver();
        filesystemService->createDirectory("/device/smbios");
        filesystemService->getFilesystem().mountVirtualDriver("/device/smbios", smBiosDriver);
    }

    if (lfb != nullptr) {
        auto *lfbNode = new Device::Graphic::LinearFrameBufferNode("lfb", lfb);
        deviceDriver->addNode("/", lfbNode);
    }

    auto *terminalNode = new Device::Graphic::TerminalNode("terminal", terminal);
    deviceDriver->addNode("/", terminalNode);

    // Open first file descriptors for Util::System::in, Util::System::out and Util::System::error
    Util::Io::File::open("/device/terminal");
    Util::Io::File::open("/device/terminal");
    Util::Io::File::open("/device/terminal");

    // Initialize ports
    LOG_INFO("Initializing ports");
    Device::SerialPort::initializeAvailablePorts();
    Device::ParallelPort::initializeAvailablePorts();

    Kernel::Log::disableEarlySerialLogging();
    if (multiboot->hasKernelOption("log_ports")) {
        const auto ports = multiboot->getKernelOption("log_ports").split(",");
        for (const auto &port : ports) {
            auto file = Util::Io::File("/device/" + port.toLowerCase());
            if (!file.exists()) {
                LOG_ERROR("Logging port [%s] not present", static_cast<const char*>(port));
                break;
            }

            auto *stream = new Util::Io::FileOutputStream(file);
            Kernel::Log::addOutputStream(*stream, Util::String(Device::Serial::portToString(Kernel::Log::getEarlyLogSerialPort().getPort())) != port.toUpperCase());
        }
    }

    // Initialize PS/2 devices
    LOG_INFO("Initializing PS/2 devices");
    auto *ps2Controller = Device::Ps2Controller::initialize();
    auto *keyboard = Device::Keyboard::initialize(*ps2Controller);
    auto *mouse = Device::Mouse::initialize(*ps2Controller);

    if (keyboard == nullptr) {
        // Register a null node as keyboard, so that the system can at least still boot up
        auto *node = new Filesystem::Memory::NullNode("keyboard");
        auto &driver = filesystemService->getFilesystem().getVirtualDriver("/device");
        driver.addNode("/", node);
    } else {
        keyboard->plugin();
    }

    if (mouse != nullptr) {
        mouse->plugin();
    }

    // Initialize network
    LOG_INFO("Initializing network stack");
    auto *networkService = new Kernel::NetworkService();
    Kernel::Service::registerService(Kernel::NetworkService::SERVICE_ID, networkService);

    networkService->initializeLoopback();

    Device::Network::Rtl8139::initializeAvailableCards();
    if (Device::FirmwareConfiguration::isAvailable() && networkService->isNetworkDeviceRegistered("eth0")) {
        // Configure eth0 for QEMU virtual network
        auto &eth0 = networkService->getNetworkDevice("eth0");
        auto &ip4Module = networkService->getNetworkStack().getIp4Module();

        auto address = Util::Network::Ip4::Ip4SubnetAddress("10.0.2.15/24");
        ip4Module.registerInterface(address, eth0);
        ip4Module.getRoutingModule().addRoute(Util::Network::Ip4::Ip4Route(address, "eth0"));
        ip4Module.getRoutingModule().addRoute(Util::Network::Ip4::Ip4Route(Util::Network::Ip4::Ip4SubnetAddress("10.0.2.15/0"), Util::Network::Ip4::Ip4Address("10.0.2.2"), "eth0"));
    }

    // Initialize sound
    LOG_INFO("Initializing sound");
    auto &driver = filesystemService->getFilesystem().getVirtualDriver("/device");
    driver.addNode("/", new Device::Sound::PcSpeakerNode("speaker"));

    if (Device::SoundBlaster::isAvailable()) {
        Device::SoundBlaster::initialize();
    }

    // Initialize power management
    LOG_INFO("Initializing power management");
    auto *powerManagementService = new Kernel::PowerManagementService();
    Kernel::Service::registerService(Kernel::PowerManagementService::SERVICE_ID, powerManagementService);

    // Mount devices specified /system/mount_table
    auto mountFile = Util::Io::File("/system/mount_table");
    if (mountFile.exists()) {
        LOG_INFO("Mounting devices requested by '/system/mount_table'");
        auto inputStream = Util::Io::FileInputStream(mountFile);
        auto bufferedStream = Util::Io::BufferedInputStream(inputStream);
        bool endOfFile = false;

        Util::String line = bufferedStream.readLine(endOfFile);
        while (!endOfFile) {
            if (line.beginsWith("#")) {
                line = bufferedStream.readLine(endOfFile);
                continue;
            }

            auto split = line.split(" ");
            if (split.length() < 3) {
                LOG_ERROR("Invalid line in /system/mount_table");
                line = bufferedStream.readLine(endOfFile);
                continue;
            }

            LOG_INFO("Mounting [%s] to [%s]", static_cast<const char*>(split[0]), static_cast<const char*>(split[1]));
            auto success = filesystemService->mount(split[0], split[1], split[2]);
            if (!success) {
                LOG_ERROR("Failed to mount [%s] to [%s]", static_cast<const char*>(split[0]), static_cast<const char*>(split[1]));
            }

            line = bufferedStream.readLine(endOfFile);
        }
    }

    // Clear screen and print banner
    Kernel::Log::removeOutputStream(*terminal);
    Util::Graphic::Ansi::clearScreen();
    Util::Graphic::Ansi::setPosition(Util::Graphic::Ansi::CursorPosition{0, 0});

    auto bannerFile = Util::Io::File("/system/banner.txt");
    if (bannerFile.exists()) {
        auto bannerStream = Util::Io::FileInputStream(bannerFile);
        auto bufferedStream = Util::Io::BufferedInputStream(bannerStream);

        auto banner = bufferedStream.readString(bannerFile.getLength());
        Util::System::out << Util::String::format(static_cast<const char*>(banner),
                          BuildConfig::getVersion(), BuildConfig::getCodename(), BuildConfig::getBuildDate(),
                          BuildConfig::getGitBranch(), BuildConfig::getGitRevision(), static_cast<const char*>(multiboot->getBootloaderName()))
                          << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    } else {
        Util::System::out << "Welcome to hhuOS!" << Util::Io::PrintStream::endl
                          << "Version: " << BuildConfig::getVersion() << " (" << BuildConfig::getGitBranch() << ")" << Util::Io::PrintStream::endl
                          << "Git revision: " << BuildConfig::getGitRevision() << Util::Io::PrintStream::endl
                          << "Build date: " << BuildConfig::getBuildDate() << Util::Io::PrintStream::endl << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    }

    schedulerService->startScheduler();

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Once you entered the gates of hell, you are not allowed to leave!");
}

Util::HeapMemoryManager& GatesOfHell::getKernelHeap() {
    return *kernelHeap;
}

uint32_t GatesOfHell::createInitialMapping(Kernel::Paging::Table &pageDirectory, Kernel::Paging::Table *pageTableMemory, uint32_t physicalStartAddress, uint32_t virtualStartAddress, uint32_t pageCount) {
    uint32_t allocatedPageTables = 0;

    for (uint32_t i = 0; i < pageCount; i++) {
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

bool GatesOfHell::isKernelHeapInitialized() {
    return kernelHeap != nullptr;
}
