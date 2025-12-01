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

#include "GatesOfHell.h"
#include "device/cpu/Cpu.h"
#include "kernel/log/Log.h"
#include "kernel/memory/MemoryLayout.h"
#include "kernel/memory/Paging.h"
#include "kernel/memory/PagingAreaManager.h"
#include "kernel/memory/PageFrameAllocator.h"
#include "kernel/memory/VirtualAddressSpace.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/InterruptService.h"
#include "device/interrupt/apic/Apic.h"
#include "device/system/Acpi.h"
#include "kernel/service/InformationService.h"
#include "device/system/SmBios.h"
#include "device/time/pit/Pit.h"
#include "device/time/rtc/Rtc.h"
#include "kernel/service/TimeService.h"
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
#include "device/storage/ahci/AhciController.h"
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
#include "util/async/BasicRunnable.h"
#include "kernel/memory/PagingAreaManagerRefillRunnable.h"
#include "lib/util/async/Process.h"
#include "device/hid/Ps2Controller.h"
#include "device/interrupt/pic/Pic.h"
#include "device/port/serial/Serial.h"
#include "device/port/serial/SerialPort.h"
#include "device/port/serial/SimpleSerialPort.h"
#include "filesystem/Filesystem.h"
#include "kernel/multiboot/Multiboot.h"
#include "kernel/network/NetworkStack.h"
#include "kernel/network/ip4/Ip4Module.h"
#include "kernel/network/ip4/Ip4RoutingModule.h"
#include "kernel/process/Process.h"
#include "kernel/process/Thread.h"
#include "kernel/service/Service.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Panic.h"
#include "lib/util/base/FreeListMemoryManager.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/hardware/SmBios.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/ip4/Ip4Route.h"
#include "lib/util/network/ip4/Ip4SubnetAddress.h"
#include "device/system/Bios.h"
#include "kernel/process/Scheduler.h"
#include "lib/util/base/Constants.h"
#include "device/system/AdvancedPowerManagement.h"
#include "device/system/Machine.h"
#include "device/network/ne2000/Ne2000.h"
#include "filesystem/iso9660/IsoDriver.h"
#include "device/time/rtc/Cmos.h"
#include "device/graphic/VesaBiosExtensions.h"
#include "device/time/acpi/AcpiTimer.h"
#include "lib/util/time/Timestamp.h"
#include "device/time/hpet/Hpet.h"
#include "filesystem/memory/BufferNode.h"
#include "util/graphic/BitmapFile.h"
#include "kernel/log/LogNode.h"
#include "lib/util/graphic/font/Terminal8x8.h"
#include "kernel/service/CpuService.h"
#include "kernel/service/SoundService.h"
#include "kernel/sound/AudioMixer.h"

namespace Device {
class WaitTimer;
class DateProvider;
class TimeProvider;
}  // namespace Device

namespace Util {
class HeapMemoryManager;
}  // namespace Util

extern const uint32_t ___KERNEL_DATA_START__;
extern const uint32_t ___KERNEL_DATA_END__;
extern const uint32_t ___WRITE_PROTECTED_START__;
extern const uint32_t ___WRITE_PROTECTED_END__;

const uint32_t KERNEL_DATA_START = reinterpret_cast<uint32_t>(&___KERNEL_DATA_START__);
const uint32_t KERNEL_DATA_END = reinterpret_cast<uint32_t>(&___KERNEL_DATA_END__);
const uint32_t WRITE_PROTECTED_START = reinterpret_cast<uint32_t>(&___WRITE_PROTECTED_START__);
const uint32_t WRITE_PROTECTED_END = reinterpret_cast<uint32_t>(&___WRITE_PROTECTED_END__);

const constexpr uint32_t INITIAL_PAGING_AREA_SIZE = 256 * 1024;
const constexpr uint32_t INITIAL_KERNEL_HEAP_SIZE = 128 * 1024;

extern "C" {
    void _init();
}

extern "C" int32_t atexit ([[maybe_unused]] void (*func)()) noexcept {
    return 0;
}

Util::HeapMemoryManager *GatesOfHell::kernelHeap = nullptr;

extern "C" void main(uint32_t multibootMagic, const Kernel::Multiboot *multiboot) {
    GatesOfHell::enter(multibootMagic, multiboot);
}

void GatesOfHell::enter(uint32_t multibootMagic, const Kernel::Multiboot *multiboot) {
    // Check multiboot magic number
    LOG_INFO("Welcome to hhuOS early boot environment!");
    if (multibootMagic != Kernel::Multiboot::MAGIC) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Multiboot2 magic number is invalid!");
    }

    // Needed later for mapping the multiboot header into virtual memory
    auto multibootAddress = reinterpret_cast<uint32_t>(multiboot);
    auto multibootSize = multiboot->getSize();

    // Check if multiboot struct interferes with BIOS or SMP code
    if (multibootAddress < Kernel::MemoryLayout::USABLE_LOWER_MEMORY.startAddress) {
        multibootAddress = Util::Address(multibootAddress + multibootSize).alignUp(Util::PAGESIZE).get();
        if (multibootAddress < Kernel::MemoryLayout::USABLE_LOWER_MEMORY.startAddress) {
            multibootAddress = Kernel::MemoryLayout::USABLE_LOWER_MEMORY.startAddress;
        }

        Util::Address(multibootAddress).copyRange(Util::Address(multiboot), multiboot->getSize());
        multiboot = reinterpret_cast<const Kernel::Multiboot*>(multibootAddress);
    }

    // Scan memory map
    LOG_INFO("Scanning memory map to calculate physical address limit");
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
            bootstrapMemory = Util::Address(multibootAddress + multibootSize).alignUp(Util::PAGESIZE).get();
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
    const auto kernelSize = Util::Address(KERNEL_DATA_END - KERNEL_DATA_START).alignUp(Util::PAGESIZE).get();
    pageTableMemory += createInitialMapping(*pageDirectory, pageTableMemory, KERNEL_DATA_START, KERNEL_DATA_START, kernelSize / Util::PAGESIZE);

    // Map beginning of paging area
    const auto pagingAreaVirtual = Util::Address(KERNEL_DATA_END).alignUp(Util::PAGESIZE).get();
    pageTableMemory += createInitialMapping(*pageDirectory, pageTableMemory, pagingAreaPhysical, pagingAreaVirtual, INITIAL_PAGING_AREA_SIZE / Util::PAGESIZE);

    // Map beginning of kernel heap
    const auto kernelHeapVirtual = pagingAreaVirtual + Kernel::MemoryLayout::PAGING_AREA_SIZE;
    pageTableMemory += createInitialMapping(*pageDirectory, pageTableMemory, kernelHeapPhysical, kernelHeapVirtual, INITIAL_KERNEL_HEAP_SIZE / Util::PAGESIZE);

    // Copy memory map to end of initial kernel heap area (will be overwritten later on!)
    auto memoryMapTarget = Util::Address((kernelHeapPhysical + INITIAL_KERNEL_HEAP_SIZE) - memoryMap->tagHeader.size);
    memoryMapTarget.copyRange(Util::Address(memoryMap), memoryMap->tagHeader.size);
    memoryMap = reinterpret_cast<Kernel::Multiboot::MemoryMapHeader*>((kernelHeapVirtual + INITIAL_KERNEL_HEAP_SIZE) - memoryMap->tagHeader.size);

    // Enable paging
    LOG_INFO("Enabling paging");
    Kernel::Paging::loadDirectory(*pageDirectory);
    Device::Cpu::writeCr0(Device::Cpu::readCr0() | Device::Cpu::PAGING);

    // Initialize kernel heap
    LOG_INFO("Initializing kernel heap");
    static Util::FreeListMemoryManager kernelHeapManager(reinterpret_cast<void*>(kernelHeapVirtual), reinterpret_cast<void*>(Kernel::MemoryLayout::KERNEL_HEAP_END_ADDRESS));
    kernelHeap = &kernelHeapManager;

    LOG_INFO("Kernel heap initialized (Bootstrap memory: [0x%08x])", bootstrapMemory);

    // Initialize GDT
    LOG_INFO("Initializing CPU service");
    auto *cpuService = new Kernel::CpuService();

    LOG_INFO("Loading Global Descriptor Table");
    cpuService->loadGdt();

    // Initialize Paging Area Manager -> Manages the virtual addresses of all page tables and directories
    LOG_INFO("Initializing paging area manager");
    auto usedPagingAreaPages = (reinterpret_cast<uint32_t>(pageTableMemory) - pagingAreaPhysical) / Util::PAGESIZE;
    auto *pagingAreaManager = new Kernel::PagingAreaManager(reinterpret_cast<uint8_t*>(pagingAreaVirtual), INITIAL_PAGING_AREA_SIZE / Util::PAGESIZE, usedPagingAreaPages);

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
            entry.set(reinterpret_cast<uint32_t>(pagingAreaVirtual + Util::PAGESIZE + (Util::PAGESIZE * updatedEntries++)), entry.getFlags());
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

    // Initialize the classic PIC and interrupt service
    // Needs to be done before memory service initialization, so that the memory service can register its system calls
    LOG_INFO("Initializing interrupt service");
    auto *interruptService = new Kernel::InterruptService();
    Kernel::Service::registerService(Kernel::InterruptService::SERVICE_ID, interruptService);
    Kernel::Service::registerService(Kernel::CpuService::SERVICE_ID, cpuService);

    LOG_INFO("Loading interrupt descriptor table");
    interruptService->loadIdt();

    // Create kernel address space and memory service
    LOG_INFO("Initializing kernel address space");
    auto *kernelAddressSpace = new Kernel::VirtualAddressSpace(pageDirectory, virtualPageDirectory, *kernelHeap);
    auto *memoryService = new Kernel::MemoryService(pageFrameAllocator, pagingAreaManager, kernelAddressSpace);

    // The memory service and IDT are initialized and after registering the memory service, page faults can be handled, allowing us to fully use the kernel heap
    Kernel::Service::registerService(Kernel::MemoryService::SERVICE_ID, memoryService);

    // Allocate page tables for the whole kernel area
    for (uint32_t i = 0; i < Kernel::MemoryLayout::KERNEL_END / (Util::PAGESIZE * 1024); i++) {
        if ((*pageDirectory)[i].isUnused()) {
            pagingAreaManager->refillPool(); // Make sure the paging area manager does not run out of page tables
            auto *virtualPageTable = memoryService->allocatePageTable();
            auto *physicalPageTable = memoryService->getPhysicalAddress(virtualPageTable);

            (*virtualPageDirectory)[i].set(reinterpret_cast<uint32_t>(virtualPageTable), Kernel::Paging::PRESENT | Kernel::Paging::WRITABLE);
            (*pageDirectory)[i].set(reinterpret_cast<uint32_t>(physicalPageTable), Kernel::Paging::PRESENT | Kernel::Paging::WRITABLE);
        }
    }

    // Map Multiboot2 tags
    const auto multibootStartPage = reinterpret_cast<uint32_t>(multiboot) & 0xfffff000;
    const auto multibootEndPage = (reinterpret_cast<uint32_t>(multiboot) + multibootSize + Util::PAGESIZE - 1) & 0xfffff000;
    const auto multibootPages = (multibootEndPage - multibootStartPage) / Util::PAGESIZE;
    const auto multibootPageOffset = reinterpret_cast<uint32_t>(multiboot) % Util::PAGESIZE;
    const auto *multibootVirtualAddress = static_cast<const uint8_t*>(memoryService->mapIO(const_cast<void*>(reinterpret_cast<const void*>(multiboot)), multibootPages, true)) + multibootPageOffset;
    multiboot = reinterpret_cast<const Kernel::Multiboot*>(multibootVirtualAddress);

    // Reserve Multiboot2 modules, before any page faults on the heap occur.
    // Otherwise, parts of the modules might be overwritten
    for (const auto &name : multiboot->getModuleNames()) {
        auto &module = multiboot->getModule(name);
        pageFrameAllocator->setMemory(reinterpret_cast<uint8_t*>(module.startAddress), reinterpret_cast<uint8_t*>(module.endAddress - 1), 0, true);
    }

    // Identity map BIOS related parts of the lower 1 MiB
    // Depending on the system and bootloader, this may be necessary to initialize ACPI and SMBIOS data structures
    // The BIOS call code and startup code for application processors is also located there
    memoryService->mapPhysical(nullptr, nullptr, Kernel::MemoryLayout::USABLE_LOWER_MEMORY.startAddress / Util::PAGESIZE, Kernel::Paging::PRESENT | Kernel::Paging::WRITABLE);
    memoryService->mapPhysical(reinterpret_cast<void*>(Kernel::MemoryLayout::USABLE_LOWER_MEMORY.endAddress + 1), reinterpret_cast<void*>(Kernel::MemoryLayout::USABLE_LOWER_MEMORY.endAddress + 1),
                               (KERNEL_DATA_START - (Kernel::MemoryLayout::USABLE_LOWER_MEMORY.endAddress + 1)) / Util::PAGESIZE, Kernel::Paging::PRESENT | Kernel::Paging::WRITABLE);

    // Set log level
    if (multiboot->hasKernelOption("log_level")) {
        const auto level = multiboot->getKernelOption("log_level");
        Kernel::Log::setLevel(level);
    }

    // Memory management has been set up now, and we continue with the remaining boot process
    LOG_INFO("Welcome to hhuOS!");
    LOG_INFO("Used kernel heap memory during early boot process: [%u KiB]", (kernelHeapManager.getTotalMemory() - kernelHeapManager.getFreeMemory()) / 1024);

    // Create scheduler and process service and register kernel process
    LOG_INFO("Initializing scheduler");
    auto *kernelProcess = new Kernel::Process(*kernelAddressSpace, "Kernel");
    auto *processService = new Kernel::ProcessService(kernelProcess);
    auto &scheduler = processService->getScheduler();
    Kernel::Service::registerService(Kernel::ProcessService::SERVICE_ID, processService);

    // Initialize frame buffer
    LOG_INFO("Initializing display");
    Util::Graphic::LinearFrameBuffer *lfb = nullptr;
    Util::Graphic::Terminal *terminal;
    if (multiboot->hasTag(Kernel::Multiboot::FRAMEBUFFER_INFO)) {
        const auto &tag = multiboot->getTag<Kernel::Multiboot::FramebufferInfo>(Kernel::Multiboot::FRAMEBUFFER_INFO);

        if (tag.type == Kernel::Multiboot::EGA_TEXT) {
            LOG_INFO("Framebuffer info provided by bootloader (Type: [EGA_TEXT], Resolution: [%ux%u@%u], Address: [0x%08x])", tag.width, tag.height, tag.bpp, tag.address);

            if ((tag.width != 80 && tag.width != 40) && tag.height != 25) {
                Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Unsupported text mode resolution!");
            }

            terminal = new Device::Graphic::ColorGraphicsAdapter(tag.width, tag.height);
        } else if (tag.type == Kernel::Multiboot::RGB) {
            LOG_INFO("Framebuffer info provided by bootloader (Type: [RGB], Resolution: [%ux%u@%u], Address: [0x%08x])", tag.width, tag.height, tag.bpp, tag.address);

            lfb = new Util::Graphic::LinearFrameBuffer(tag.address, tag.width, tag.height, tag.bpp, tag.pitch);
            terminal = new Util::Graphic::LinearFrameBufferTerminal(*lfb);
        } else {
            Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Unsupported frame buffer type!");
        }
    } else {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "No framebuffer information provided by bootloader!");
    }

    Kernel::Log::addOutputStream(*terminal);

    // Detect CPU features
    if (Util::Hardware::CpuId::isAvailable()) {
        const auto info = Util::Hardware::CpuId::getCpuInfo();
        LOG_INFO("CPU info: Vendor [%s], Family [%u], Model [%u], Stepping [%u], Type [%u]", static_cast<const char*>(Util::Hardware::CpuId::getVendorString()), info.family, info.model, info.stepping, info.type);

        Util::String featureString;
        for (const auto &feature : info.getFeaturesAsArray()) {
            featureString += Util::Hardware::CpuId::getFeatureAsString(feature) + Util::String(" ");
        }
        LOG_INFO("CPU features: %s", static_cast<const char*>(featureString));
    } else {
        LOG_ERROR("CPUID not available!");
    }

    // Detect Multiboot2 tags
    LOG_INFO("Bootloader: [%s], Multiboot size: [%u Byte]", static_cast<const char*>(multiboot->getBootloaderName()), multiboot->getSize());
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
    if (smBios->getTables().hasTable(Util::Hardware::SmBios::BIOS_INFORMATION)) {
        auto &biosInformation = reinterpret_cast<const Util::Hardware::SmBios::BiosInformation&>(smBios->getTables()[Util::Hardware::SmBios::BIOS_INFORMATION]);
        LOG_INFO("BIOS vendor: [%s], BIOS version: [%s (%s)]", biosInformation.getVendorName(), biosInformation.getVersion(), biosInformation.getReleaseDate());
    }

    // Initialize BIOS calls
    if (Device::Bios::isAvailable()) {
        LOG_INFO("Initializing BIOS calls");
        Device::Bios::initialize();
    }

    // Initialize power management (APM needs BIOS calls)
    LOG_INFO("Initializing power management");
    Device::Machine *machine = nullptr;
    if (multiboot->getKernelOption("apm", "true") == "true" && Device::AdvancedPowerManagement::isAvailable()) {
        LOG_INFO("APM is available");
        machine = Device::AdvancedPowerManagement::initialize();
    }

    if (machine == nullptr) {
        machine = new Device::Machine();
    }

    auto *powerManagementService = new Kernel::PowerManagementService(machine);
    Kernel::Service::registerService(Kernel::PowerManagementService::SERVICE_ID, powerManagementService);

    Device::Graphic::VesaBiosExtensions *vbe = nullptr;
    if (multiboot->getKernelOption("vbe", "true") == "true" && Device::Graphic::VesaBiosExtensions::isAvailable()) {
        LOG_INFO("Initializing Vesa BIOS Extensions");
        vbe = Device::Graphic::VesaBiosExtensions::initialize();

        if (multiboot->hasKernelOption("resolution")) {
            auto split1 = multiboot->getKernelOption("resolution").split("@");
            auto split2 = split1[0].split("x");

            if (split2.length() < 2) {
                LOG_WARN("Ignoring invalid 'resolution' parameter");
            } else {
                auto resolutionX = Util::String::parseNumber<uint16_t>(split2[0]);
                auto resolutionY = Util::String::parseNumber<uint16_t>(split2[1]);
                uint8_t colorDepth = split1.length() > 1 ? Util::String::parseNumber<uint8_t>(split1[1]) : 32;

                if (lfb == nullptr || (lfb->getResolutionX() != resolutionX || lfb->getResolutionY() != resolutionY || lfb->getColorDepth() != colorDepth)) {
                    Kernel::Log::removeOutputStream(*terminal);
                    delete terminal;

                    auto mode = vbe->findMode(resolutionX, resolutionY, colorDepth);
                    Device::Graphic::VesaBiosExtensions::setMode(mode.modeNumber);

                    lfb = new Util::Graphic::LinearFrameBuffer(mode.physicalAddress, mode.resolutionX, mode.resolutionY, mode.colorDepth, mode.pitch);
                    terminal = new Util::Graphic::LinearFrameBufferTerminal(*lfb);
                    Kernel::Log::addOutputStream(*terminal);
                }
            }
        }
    }

    auto *lfbNode = lfb == nullptr ? nullptr : new Device::Graphic::LinearFrameBufferNode("lfb", *lfb, vbe);

    // Set up a timer device for busy wait functionality.
    // Needs to be done before PIC/APIC initialization, since they call 'TimeService::busyWait()'.
    // System timer and date provider can be initialized later on
    LOG_INFO("Searching for timer device suitable for busy waiting");
    Device::WaitTimer *waitTimer = nullptr;

    if (Device::Hpet::isAvailable()) {
        LOG_INFO("Using HPET for busy waiting");
        waitTimer = new Device::Hpet();
    } else if (Device::AcpiTimer::isAvailable()) {
        LOG_INFO("Using ACPI timer for busy waiting");
        waitTimer = new Device::AcpiTimer();
    } else {
        LOG_INFO("Using PIT for busy waiting");
        waitTimer = new Device::Pit();
    }

    auto *timeService = new Kernel::TimeService(waitTimer);
    Kernel::Service::registerService(Kernel::TimeService::SERVICE_ID, timeService);

    // Initialize classic PIC
    LOG_INFO("Initializing PIC");
    auto *pic = new Device::Pic();
    interruptService->usePic(pic);

    // Check if APIC exists and initialize it
    if (multiboot->getKernelOption("apic", "true") == "true" && Device::Apic::isAvailable()) {
        LOG_INFO("APIC detected");
        auto *apic = Device::Apic::initialize();
        if (apic == nullptr) {
            LOG_WARN("Failed to initialize APIC -> Falling back to PIC");
        } else {
            interruptService->useApic(apic);
            apic->startCurrentTimer();

            if (apic->isSymmetricMultiprocessingSupported()) {
                cpuService->startupApplicationProcessors();
            }
        }
    } else {
        LOG_INFO("APIC not available -> Falling back to PIC");
    }

    // Create thread to refill block pool of paging area manager
    auto &refillThread = Kernel::Thread::createKernelThread("Paging-Area-Pool-Refiller", processService->getKernelProcess(), new Kernel::PagingAreaManagerRefillRunnable(*pagingAreaManager));
    scheduler.ready(refillThread);

    // Protect kernel code
    for (uint32_t address = WRITE_PROTECTED_START; address < WRITE_PROTECTED_END; address += Util::PAGESIZE) {
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
    Device::Cmos::enableNmi();

    // Set up system timer
    LOG_INFO("Searching for timer device suitable as system timer");
    Device::TimeProvider *systemTimer = nullptr;

    if (Device::Hpet::isAvailable()) {
        LOG_INFO("Using HPET as system timer");
        Device::Pit::disable();
        auto *hpet = reinterpret_cast<Device::Hpet*>(waitTimer);
        hpet->pluginSystemTimer();
        systemTimer = hpet;
    } else if (Device::AcpiTimer::isAvailable()) {
        LOG_INFO("Using PIT as system timer");
        auto *pit = new Device::Pit();
        pit->setInterruptRate(Util::Time::Timestamp::ofMilliseconds(1), Util::Time::Timestamp::ofMilliseconds(10));
        pit->plugin();
        systemTimer = pit;
    } else {
        LOG_INFO("Using PIT as system timer");
        auto *pit = (Device::Pit*) waitTimer;
        pit->setInterruptRate(Util::Time::Timestamp::ofMilliseconds(1), Util::Time::Timestamp::ofMilliseconds(10));
        pit->plugin();
        systemTimer = pit;
    }
    timeService->setTimeProvider(systemTimer);

    // Set up date provider
    LOG_INFO("Searching for date provider device");
    Device::DateProvider *dateProvider = nullptr;

    if (Device::Rtc::isAvailable()) {
        LOG_INFO("Using RTC as date provider");
        auto *rtc = new Device::Rtc();
        rtc->plugin();

        if (!Device::Rtc::isValid()) {
            LOG_WARN("CMOS has been cleared -> RTC is probably providing invalid date and time");
        }

        dateProvider = rtc;
    } else {
        LOG_ERROR("No date provider available");
    }
    timeService->setDateProvider(dateProvider);

    // Scan PCI bus
    Device::Pci::scan();

    // Initialize storage devices
    auto *storageService = new Kernel::StorageService();
    Kernel::Service::registerService(Kernel::StorageService::SERVICE_ID, storageService);

    LOG_INFO("Searching multiboot modules for virtual disk drive");
    for (const auto &name : multiboot->getModuleNames()) {
        if (name.beginsWith("vdd")) {
            auto &module = multiboot->getModule(name);
            const auto moduleSize = module.endAddress - module.startAddress;
            const auto modulePageOffset = module.startAddress % Util::PAGESIZE;
            const auto modulePageCount = moduleSize % Util::PAGESIZE == 0 ? (moduleSize / Util::PAGESIZE) : (moduleSize / Util::PAGESIZE) + 1;
            const auto sectorCount = moduleSize % 512 == 0 ? (moduleSize / 512) : (moduleSize / 512) + 1;

            auto *moduleVirtualAddress = memoryService->mapIO(reinterpret_cast<void *>(module.startAddress), modulePageCount);
            auto *device = new Device::Storage::VirtualDiskDrive(static_cast<uint8_t *>(moduleVirtualAddress) + modulePageOffset, 512, sectorCount);

            storageService->registerDevice(device, "vdd");
        }
    }

    Device::Storage::IdeController::initializeAvailableControllers();
    Device::Storage::AhciController::initializeAvailableControllers();

    if (Device::Storage::FloppyController::isAvailable()) {
        auto *floppyController = new Device::Storage::FloppyController();
        floppyController->initializeAvailableDrives();
    }

    // Initialize filesystem
    LOG_INFO("Initializing filesystem");
    auto *filesystemService = new Kernel::FilesystemService();
    Kernel::Service::registerService(Kernel::FilesystemService::SERVICE_ID, filesystemService);

    Util::Reflection::InstanceFactory::registerPrototype(new Filesystem::Fat::FatDriver());
    Util::Reflection::InstanceFactory::registerPrototype(new Filesystem::Iso::IsoDriver());

    if (!multiboot->hasKernelOption("root")) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "No root filesystem specified!");
    }

    auto rootOptions = multiboot->getKernelOption("root").split(",");
    if (rootOptions.length() < 2) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Invalid arguments for root filesystem!");
    }

    const auto &rootDeviceName = rootOptions[0];
    const auto &rootDriverName = rootOptions[1];
    if (!storageService->isDeviceRegistered(rootDeviceName)) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Root device not found!");
    }

    LOG_INFO("Mounting [%s] to root using driver [%s]", static_cast<const char*>(rootDeviceName), static_cast<const char*>(rootDriverName));
    if (!filesystemService->mount(rootDeviceName, "/", rootDriverName)) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Failed to mount root filesystem!");
    }

    auto *deviceDriver = new Filesystem::Memory::MemoryDriver();
    filesystemService->createDirectory("/device");
    filesystemService->getFilesystem().mountVirtualDriver("/device", deviceDriver);

    auto *tempDriver = new Filesystem::Memory::MemoryDriver();
    filesystemService->createDirectory("/temp");
    filesystemService->getFilesystem().mountVirtualDriver("/temp", tempDriver);

    auto *processDriver = new Filesystem::Process::ProcessDriver();
    filesystemService->createDirectory("/process");
    filesystemService->getFilesystem().mountVirtualDriver("/process", processDriver);

    deviceDriver->addNode("/", new Filesystem::Memory::NullNode());
    deviceDriver->addNode("/", new Filesystem::Memory::ZeroNode());
    deviceDriver->addNode("/", new Filesystem::Memory::RandomNode());
    deviceDriver->addNode("/", new Filesystem::Memory::MountsNode());
    deviceDriver->addNode("/", new Kernel::LogNode());
    deviceDriver->addNode("/", new Kernel::MemoryStatusNode());

    if (Device::FirmwareConfiguration::isAvailable()) {
        auto *fwCfg = new Device::FirmwareConfiguration();
        auto *qemuDriver = new Filesystem::Qemu::FirmwareConfigurationDriver(*fwCfg);
        filesystemService->createDirectory("/device/qemu");
        filesystemService->getFilesystem().mountVirtualDriver("/device/qemu", qemuDriver);
    }

    const auto &acpiTables = acpi->getTables();
    if (acpiTables.getTableCount() != 0) {
        auto *acpiDriver = new Filesystem::Acpi::AcpiDriver();
        filesystemService->createDirectory("/device/acpi");
        filesystemService->getFilesystem().mountVirtualDriver("/device/acpi", acpiDriver);

        if (acpiTables.hasTable("BGRT")) {
            const auto &bgrt = reinterpret_cast<const Util::Hardware::Acpi::Bgrt&>(acpiTables["BGRT"]);
            const auto imagePageOffset = bgrt.imageAddress % Util::PAGESIZE;
            const auto *imagePage = static_cast<const uint8_t*>(memoryService->mapIO(const_cast<void*>(reinterpret_cast<const void*>(bgrt.imageAddress)), 1));
            const auto *imageVirtual = imagePage + imagePageOffset;
            const auto bitmapSize = reinterpret_cast<const Util::Graphic::BitmapFile::Header*>(imageVirtual)->size;
            if (bitmapSize > Util::PAGESIZE) {
                auto pageCount = bitmapSize % Util::PAGESIZE == 0 ? (bitmapSize / Util::PAGESIZE) : (bitmapSize / Util::PAGESIZE) + 1;
                delete imagePage;
                imagePage = static_cast<const uint8_t*>(memoryService->mapIO(const_cast<void*>(reinterpret_cast<const void*>(bgrt.imageAddress)), pageCount));
                imageVirtual = imagePage + imagePageOffset;
            }

            auto *bootLogoNode = new Filesystem::Memory::BufferNode("bootlogo.bmp", imageVirtual, bitmapSize);
            deviceDriver->addNode("/", bootLogoNode);
        }
    }

    if (smBios->getTables().getTableCount() != 0) {
        auto *smBiosDriver = new Filesystem::SmBios::SmBiosDriver();
        filesystemService->createDirectory("/device/smbios");
        filesystemService->getFilesystem().mountVirtualDriver("/device/smbios", smBiosDriver);
    }

    if (lfb != nullptr) {
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
            if (!Util::Io::File("/device/" + port.toLowerCase()).exists()) {
                LOG_ERROR("Logging port [%s] not present", static_cast<const char*>(port));
                break;
            }

            if (port.toLowerCase().beginsWith("com")) {
                auto comPort = Device::Serial::portFromString(port);
                auto *serialPort = new Device::SerialPort(comPort);
                bool append = Util::String(Device::Serial::portToString(Kernel::Log::getEarlyLogSerialPort().getPort())) != port.toUpperCase();
                Kernel::Log::addOutputStream(*serialPort, append);
            } else if (port.toLowerCase().beginsWith("lpt")) {
                auto lptPort = Device::ParallelPort::portFromString(port);
                auto *parallelPort = new Device::ParallelPort(lptPort);
                Kernel::Log::addOutputStream(*parallelPort);
            }
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
    Device::Network::Ne2000::initializeAvailableCards();

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
    auto *soundService = new Kernel::SoundService();
    Kernel::Service::registerService(Kernel::SoundService::SERVICE_ID, soundService);

    auto &driver = filesystemService->getFilesystem().getVirtualDriver("/device");
    driver.addNode("/", new Device::PcSpeakerNode("speaker"));

    Device::SoundBlaster::initialize();

    // Mount devices specified /system/mount_table
    auto mountFile = Util::Io::File("/system/mount_table");
    if (mountFile.exists()) {
        LOG_INFO("Mounting devices requested by '/system/mount_table'");
        auto inputStream = Util::Io::FileInputStream(mountFile);
        auto bufferedStream = Util::Io::BufferedInputStream(inputStream);

        auto line = bufferedStream.readLine();
        while (!line.endOfFile) {
            if (line.content.beginsWith("#")) {
                line = bufferedStream.readLine();
                continue;
            }

            auto split = line.content.split(" ");
            if (split.length() < 3) {
                LOG_ERROR("Invalid line in /system/mount_table");
                line = bufferedStream.readLine();
                continue;
            }

            LOG_INFO("Mounting [%s] to [%s]", static_cast<const char*>(split[0]), static_cast<const char*>(split[1]));
            filesystemService->createDirectory(split[1]);

            auto success = filesystemService->mount(split[0], split[1], split[2]);
            if (!success) {
                LOG_ERROR("Failed to mount [%s] to [%s]", static_cast<const char*>(split[0]), static_cast<const char*>(split[1]));
            }

            line = bufferedStream.readLine();
        }
    }

    // Ready 'shell' process
    Util::Async::Process::execute(Util::Io::File("/bin/shell"), Util::Io::File("/device/terminal"), Util::Io::File("/device/terminal"), Util::Io::File("/device/terminal"), "uptime", Util::Array<Util::String>(0));

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
                          BuildConfig::getVersion(), BuildConfig::getCodename(), BuildConfig::getBuildDate(), BuildConfig::getBuildType(),
                          BuildConfig::getGitBranch(), BuildConfig::getGitRevision(), static_cast<const char*>(multiboot->getBootloaderName()))
                          << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    } else {
        Util::System::out << "Welcome to hhuOS!" << Util::Io::PrintStream::ln
                          << "Version: " << BuildConfig::getVersion() << " (" << BuildConfig::getCodename() << ")" << Util::Io::PrintStream::ln
                          << "Build Date: " << BuildConfig::getBuildDate() << " (" << BuildConfig::getBuildType() << ")" << Util::Io::PrintStream::ln
                          << "Git Branch: " << BuildConfig::getGitBranch() << Util::Io::PrintStream::ln
                          << "Git Commit: " << BuildConfig::getGitRevision() << Util::Io::PrintStream::ln
                          << "Bootloader: " << multiboot->getBootloaderName() << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    }

    if (interruptService->usesApic() && Device::Bios::isAvailable() && !Device::FirmwareConfiguration::isAvailable()) {
        Util::System::out << Util::Graphic::Ansi::FOREGROUND_YELLOW
            << "Warning: Both APIC and BIOS calls are enabled!" << Util::Io::PrintStream::ln
            << "Changing display resolution on real hardware will probably cause the system to hang." << Util::Io::PrintStream::ln
            << "Pass 'apic=false' as a boot parameter, if you need VBE support." << Util::Io::PrintStream::ln
            << Util::Graphic::Ansi::RESET << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    }

    memoryService->enableSlabAllocator();

    LOG_INFO("Starting scheduler");
    processService->startScheduler();

    Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Once you entered the gates of hell, you are not allowed to leave!");
}

Util::HeapMemoryManager& GatesOfHell::getKernelHeap() {
    return *kernelHeap;
}

uint32_t GatesOfHell::createInitialMapping(Kernel::Paging::Table &pageDirectory, Kernel::Paging::Table *pageTableMemory, uint32_t physicalStartAddress, uint32_t virtualStartAddress, uint32_t pageCount) {
    uint32_t allocatedPageTables = 0;

    for (uint32_t i = 0; i < pageCount; i++) {
        auto physicalAddress = physicalStartAddress + i * Util::PAGESIZE;
        auto virtualAddress = virtualStartAddress + i * Util::PAGESIZE;

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
