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

#include <cstdint>

#include "device/bios/Bios.h"
#include "device/graphic/lfb/vesa/VesaBiosExtensions.h"
#include "kernel/multiboot/MultibootLinearFrameBufferProvider.h"
#include "device/graphic/terminal/lfb/LinearFrameBufferTerminalProvider.h"
#include "device/graphic/terminal/cga/ColorGraphicsAdapterProvider.h"
#include "lib/util/reflection/InstanceFactory.h"
#include "kernel/system/System.h"
#include "kernel/multiboot/Multiboot.h"
#include "kernel/multiboot/MultibootTerminalProvider.h"
#include "device/hid/Keyboard.h"
#include "lib/util/stream/InputStreamReader.h"
#include "lib/util/file/tar/Archive.h"
#include "filesystem/tar/ArchiveDriver.h"
#include "lib/util/file/File.h"
#include "lib/util/stream/BufferedReader.h"
#include "kernel/service/FilesystemService.h"
#include "filesystem/memory/MemoryDriver.h"
#include "lib/util/stream/FileInputStream.h"
#include "lib/util/cpu/CpuId.h"
#include "device/port/parallel/ParallelPort.h"
#include "lib/util/stream/FileOutputStream.h"
#include "lib/util/async/Process.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/SchedulerService.h"
#include "kernel/memory/MemoryStatusNode.h"
#include "device/power/apm/ApmMachine.h"
#include "kernel/service/PowerManagementService.h"
#include "device/pci/Pci.h"
#include "device/storage/floppy/FloppyController.h"
#include "device/storage/ide/IdeController.h"
#include "kernel/service/StorageService.h"
#include "filesystem/fat/FatDriver.h"
#include "device/sound/speaker/PcSpeakerNode.h"
#include "filesystem/memory/NullNode.h"
#include "filesystem/memory/ZeroNode.h"
#include "filesystem/memory/RandomNode.h"
#include "filesystem/process/ProcessDriver.h"
#include "device/hid/Mouse.h"
#include "device/hid/Ps2Controller.h"
#include "lib/util/stream/FileReader.h"
#include "filesystem/memory/MountsNode.h"
#include "device/debug/FirmwareConfiguration.h"
#include "filesystem/qemu/FirmwareConfigurationDriver.h"
#include "device/network/loopback/Loopback.h"
#include "kernel/service/NetworkService.h"
#include "BuildConfig.h"
#include "GatesOfHell.h"
#include "device/power/acpi/Acpi.h"
#include "asm_interface.h"
#include "device/graphic/lfb/LinearFrameBufferProvider.h"
#include "device/graphic/terminal/TerminalProvider.h"
#include "device/port/serial/SerialPort.h"
#include "device/power/default/DefaultMachine.h"
#include "filesystem/core/Filesystem.h"
#include "kernel/log/Logger.h"
#include "lib/util/Exception.h"
#include "lib/util/data/Array.h"
#include "lib/util/memory/String.h"
#include "lib/util/stream/PrintWriter.h"
#include "lib/util/system/System.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/ip4/Ip4NetworkMask.h"
#include "kernel/network/ip4/Ip4Module.h"
#include "kernel/network/NetworkStack.h"
#include "kernel/network/ip4/Ip4Route.h"

namespace Device {
class Machine;
}  // namespace Device

Kernel::Logger GatesOfHell::log = Kernel::Logger::get("GatesOfHell");

void GatesOfHell::enter() {
    const auto logLevel = Kernel::Multiboot::hasKernelOption("log_level") ? Kernel::Multiboot::getKernelOption("log_level") : "info";
    Kernel::Logger::setLevel(logLevel);

    auto &bootloaderCopyInformation = Kernel::Multiboot::getCopyInformation();
    if (!bootloaderCopyInformation.success) {
        log.error("Bootloader information has not been copied successfully -> Undefined behaviour may occur...");
    }
    log.info("Bootloader: [%s], Multiboot info size: [%u/%u Byte]", static_cast<const char*>(Kernel::Multiboot::getBootloaderName()), bootloaderCopyInformation.copiedBytes, bootloaderCopyInformation.targetAreaSize);
    log.info("%u MiB of physical memory detected", Kernel::System::getService<Kernel::MemoryService>().getMemoryStatus().totalPhysicalMemory / 1024 / 1024);

    if (Util::Cpu::CpuId::isAvailable()) {
        log.info("CPU vendor: %s", static_cast<const char*>(Util::Cpu::CpuId::getVendorString()));

        const auto info = Util::Cpu::CpuId::getCpuInfo();
        log.info("CPU info: Family [%u], Model [%u], Stepping [%u], type [%u]", info.family, info.model, info.stepping, info.type);

        const auto features = Util::Cpu::CpuId::getCpuFeatures();
        Util::Memory::String featureString;
        for (uint32_t i = 0; i < features.length(); i++) {
            featureString += Util::Cpu::CpuId::getFeatureAsString(features[i]);
            if (i < features.length() - 1) {
                featureString += ",";
            }
        }
        log.info("CPU features: %s", static_cast<const char*>(featureString));
    }

    if (Device::Acpi::isAvailable()) {
        const auto &acpiCopyInformation = Device::Acpi::getCopyInformation();
        log.info("ACPI support detected (Table size: [%u/%u Byte])", acpiCopyInformation.copiedBytes, acpiCopyInformation.targetAreaSize);

        const auto &rsdp = Device::Acpi::getRsdp();
        const auto vendor = Util::Memory::String(reinterpret_cast<const uint8_t*>(rsdp.oemId), sizeof(Device::Acpi::Rsdp::oemId));
        log.info("ACPI vendor: [%s], ACPI version: [%s]", static_cast<const char*>(vendor), rsdp.revision == 0 ? "1.0" : ">=2.0");

        const auto tables = Device::Acpi::getAvailableTables();
        Util::Memory::String featureString;
        for (uint32_t i = 0; i < tables.length(); i++) {
            featureString += tables[i];
            if (i < tables.length() - 1) {
                featureString += ",";
            }
        }
        log.info("ACPI tables: %s", static_cast<const char*>(featureString));
    }

    if (Device::Bios::isAvailable()) {
        log.info("BIOS detected");
        Device::Bios::init();
    }

    Device::Pci::scan();

    initializeStorage();

    initializeFilesystem();

    initializePorts();

    initializeTerminal();

    initializePs2Devices();

    initializeNetwork();

    initializePowerManagement();

    Kernel::Logger::addOutputStream(*new Util::Stream::FileOutputStream("/device/log"));
    enablePortLogging();

    mountDevices();

    printBanner();

    Util::Async::Process::execute(Util::File::File("/initrd/bin/shell"), Util::File::File("/device/terminal"), Util::File::File("/device/terminal"), Util::File::File("/device/terminal"), "shell", Util::Data::Array<Util::Memory::String>(0));

    log.info("Starting scheduler!");
    Kernel::System::getService<Kernel::SchedulerService>().startScheduler();

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Once you entered the gates of hell, you are not allowed to leave!");
}

void GatesOfHell::initializeTerminal() {
    log.info("Initializing graphical terminal");

    if (Device::Graphic::VesaBiosExtensions::isAvailable()) {
        log.info("VESA graphics detected");
        Util::Reflection::InstanceFactory::registerPrototype(new Device::Graphic::VesaBiosExtensions(true));
    }

    if (Device::Graphic::ColorGraphicsAdapterProvider::isAvailable()) {
        log.info("CGA graphics detected");
        Util::Reflection::InstanceFactory::registerPrototype(new Device::Graphic::ColorGraphicsAdapterProvider(true));
    }

    Device::Graphic::LinearFrameBufferProvider *lfbProvider = nullptr;
    Device::Graphic::TerminalProvider *terminalProvider;

    if (Kernel::Multiboot::hasKernelOption("lfb_provider")) {
        auto providerName = Kernel::Multiboot::getKernelOption("lfb_provider");
        log.info("LFB provider set to [%s] -> Starting initialization", static_cast<const char*>(providerName));
        lfbProvider = reinterpret_cast<Device::Graphic::LinearFrameBufferProvider*>(Util::Reflection::InstanceFactory::createInstance(providerName));
    } else if (Kernel::MultibootLinearFrameBufferProvider::isAvailable()) {
        log.info("LFB provider is not set -> Using with multiboot values");
        lfbProvider = new Kernel::MultibootLinearFrameBufferProvider();
    }

    if (lfbProvider != nullptr) {
        auto mode = lfbProvider->searchMode(800, 600, 32);
        lfbProvider->initializeLinearFrameBuffer(mode, "lfb");
    }

    if (Kernel::Multiboot::hasKernelOption("terminal_provider")) {
        auto providerName = Kernel::Multiboot::getKernelOption("terminal_provider");
        log.info("Terminal provider set to [%s] -> Starting initialization", static_cast<const char*>(providerName));
        terminalProvider = reinterpret_cast<Device::Graphic::TerminalProvider*>(Util::Reflection::InstanceFactory::createInstance(providerName));
    } else if (lfbProvider != nullptr) {
        log.info("Terminal provider is not set -> Using LFB terminal");
        auto lfbFile = Util::File::File("/device/lfb");
        terminalProvider = new Device::Graphic::LinearFrameBufferTerminalProvider(lfbFile);
    }  else if (Kernel::MultibootTerminalProvider::isAvailable()) {
        log.info("Terminal provider is not set and LFB is not available -> Using multiboot values");
        terminalProvider = new Kernel::MultibootTerminalProvider();
    } else {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Unable to find a suitable graphics driver for this machine!");
    }

    auto resolution = terminalProvider->searchMode(100, 37, 24);
    terminalProvider->initializeTerminal(resolution, "terminal");

    delete terminalProvider;
    delete lfbProvider;

    // Open first file descriptors for Util::System::in, Util::System::out and Util::System::error
    Util::File::open("/device/terminal");
    Util::File::open("/device/terminal");
    Util::File::open("/device/terminal");
}

void GatesOfHell::enablePortLogging() {
    if (!Kernel::Multiboot::hasKernelOption("log_ports")) {
        return;
    }

    const auto ports = Kernel::Multiboot::getKernelOption("log_ports").split(",");
    for (const auto &port : ports) {
        auto file = Util::File::File("/device/" + port.toLowerCase());
        if (!file.exists()) {
            log.error("Port [%s] not present", static_cast<const char*>(port));
            return;
        }

        auto *stream = new Util::Stream::FileOutputStream(file);
        Kernel::Logger::addOutputStream(*stream);
    }
}

void GatesOfHell::initializeFilesystem() {
    log.info("Initializing filesystem");
    Kernel::System::registerService(Kernel::FilesystemService::SERVICE_ID, new Kernel::FilesystemService());
    auto &filesystemService = Kernel::System::getService<Kernel::FilesystemService>();
    auto &storageService = Kernel::System::getService<Kernel::StorageService>();

    Util::Reflection::InstanceFactory::registerPrototype(new Filesystem::Fat::FatDriver());

    bool rootMounted = false;
    if (Kernel::Multiboot::hasKernelOption("root")) {
        auto rootOptions = Kernel::Multiboot::getKernelOption("root").split(",");
        if (rootOptions.length() >= 2) {
            const auto &deviceName = rootOptions[0];
            const auto &driverName = rootOptions[1];

            if (storageService.isDeviceRegistered(deviceName)) {
                log.info("Mounting [%s] to root using driver [%s]", static_cast<const char*>(deviceName), static_cast<const char*>(driverName));
                rootMounted = filesystemService.mount(deviceName, "/", driverName);
                if (!rootMounted) {
                    log.error("Failed to mount root filesystem");
                }
            } else {
                log.error("Device [%s] is not available", static_cast<const char*>(deviceName));
            }
        } else {
            log.error("Invalid options for root filesystem given");
        }
    }

    if (!rootMounted) {
        log.info("Mounting virtual filesystem as root filesystem");
        auto *rootDriver = new Filesystem::Memory::MemoryDriver();
        filesystemService.getFilesystem().mountVirtualDriver("/", rootDriver);
    }

    auto *deviceDriver = new Filesystem::Memory::MemoryDriver();
    filesystemService.createDirectory("/device");
    filesystemService.getFilesystem().mountVirtualDriver("/device", deviceDriver);

    auto *processDriver = new Filesystem::Process::ProcessDriver();
    filesystemService.createDirectory("/process");
    filesystemService.getFilesystem().mountVirtualDriver("/process", processDriver);

    filesystemService.createFile("/device/log");
    deviceDriver->addNode("/", new Filesystem::Memory::NullNode());
    deviceDriver->addNode("/", new Filesystem::Memory::ZeroNode());
    deviceDriver->addNode("/", new Filesystem::Memory::RandomNode());
    deviceDriver->addNode("/", new Filesystem::Memory::MountsNode());
    deviceDriver->addNode("/", new Kernel::MemoryStatusNode("memory"));
    deviceDriver->addNode("/", new Device::Sound::PcSpeakerNode("speaker"));

    if (Kernel::Multiboot::isModuleLoaded("initrd")) {
        log.info("Initial ramdisk detected -> Mounting [%s]", "/initrd");
        auto module = Kernel::Multiboot::getModule("initrd");
        auto *tarArchive = new Util::File::Tar::Archive(module.start);
        auto *tarDriver = new Filesystem::Tar::ArchiveDriver(*tarArchive);

        filesystemService.createDirectory("/initrd");
        filesystemService.getFilesystem().mountVirtualDriver("/initrd", tarDriver);
    }

    if (Device::FirmwareConfiguration::isAvailable()) {
        auto *fwCfg = new Device::FirmwareConfiguration();
        auto *qemuDriver = new Filesystem::Qemu::FirmwareConfigurationDriver(*fwCfg);
        filesystemService.createDirectory("/qemu");
        filesystemService.getFilesystem().mountVirtualDriver("/qemu", qemuDriver);
    }
}

void GatesOfHell::initializePs2Devices() {
    auto *ps2Controller = Device::Ps2Controller::initialize();
    auto *keyboard = Device::Keyboard::initialize(*ps2Controller);
    auto *mouse = Device::Mouse::initialize(*ps2Controller);

    if (keyboard == nullptr) {
        // Register a null node as keyboard, so that the system can at least still boot up
        auto *node = new Filesystem::Memory::NullNode("keyboard");
        auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
        auto &driver = filesystem.getVirtualDriver("/device");
        driver.addNode("/", node);
    } else {
        keyboard->plugin();
    }

    if (mouse != nullptr) {
        mouse->plugin();
    }
}

void GatesOfHell::initializePorts() {
    Device::SerialPort::initializeAvailablePorts();
    Device::ParallelPort::initializeAvailablePorts();
}

void GatesOfHell::printBanner() {
    auto bannerFile = Util::File::File("/initrd/banner.txt");
    if (bannerFile.exists()) {
        auto bannerStream = Util::Stream::FileInputStream(bannerFile);
        auto bannerReader = Util::Stream::InputStreamReader(bannerStream);
        auto bufferedReader = Util::Stream::BufferedReader(bannerReader);

        auto banner = bufferedReader.read(bannerFile.getLength());
        Util::System::out << Util::Memory::String::format(static_cast<const char*>(banner),
                                               BuildConfig::getVersion(),
                                               BuildConfig::getBuildDate(),
                                               BuildConfig::getGitBranch(),
                                               BuildConfig::getGitRevision()) << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
    } else {
        printDefaultBanner();
    }
}

void GatesOfHell::printDefaultBanner() {
    Util::System::out << "Welcome to hhuOS!" << Util::Stream::PrintWriter::endl
           << "Version: " << BuildConfig::getVersion() << " (" << BuildConfig::getGitBranch() << ")" << Util::Stream::PrintWriter::endl
           << "Git revision: " << BuildConfig::getGitRevision() << Util::Stream::PrintWriter::endl
           << "Build date: " << BuildConfig::getBuildDate() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
}

void GatesOfHell::initializePowerManagement() {
    Device::Machine *machine;
    if (Device::ApmMachine::isAvailable()) {
        log.info("APM is available");
        machine = new Device::ApmMachine();
    } else {
        machine = new Device::DefaultMachine();
    }

    auto *powerManagementService = new Kernel::PowerManagementService(machine);
    Kernel::System::registerService(Kernel::PowerManagementService::SERVICE_ID, powerManagementService);
}

void GatesOfHell::initializeStorage() {
    Device::Storage::IdeController::initializeAvailableControllers();

    if (Device::Storage::FloppyController::isAvailable()) {
        auto *floppyController = new Device::Storage::FloppyController();
        floppyController->initializeAvailableDrives();
    }
}

void GatesOfHell::initializeNetwork() {
    Kernel::System::registerService(Kernel::NetworkService::SERVICE_ID, new Kernel::NetworkService());
    auto &networkService = Kernel::System::getService<Kernel::NetworkService>();
    auto *loopback = new Device::Network::Loopback("loopback");
    networkService.registerNetworkDevice(loopback);
    networkService.getNetworkStack().getIp4Module().registerInterface(Util::Network::Ip4::Ip4Address("127.0.0.1"), Util::Network::Ip4::Ip4Address("127.0.0.0"), Util::Network::Ip4::Ip4NetworkMask(8), *loopback);
    networkService.setDefaultRoute(Kernel::Network::Ip4::Ip4Route(Util::Network::Ip4::Ip4Address("127.0.0.1"), Util::Network::Ip4::Ip4NetworkMask(8), loopback->getIdentifier()));
}

void GatesOfHell::mountDevices() {
    auto mountFile = Util::File::File("/system/mount_table");
    if (!mountFile.exists()) {
        return;
    }

    auto &filesystemService = Kernel::System::getService<Kernel::FilesystemService>();
    auto fileReader = Util::Stream::FileReader(mountFile);
    auto reader = Util::Stream::BufferedReader(fileReader);

    Util::Memory::String line = reader.readLine();
    while (!line.isEmpty()) {
        if (line.beginsWith("#")) {
            line = reader.readLine();
            continue;
        }

        auto split = line.split(" ");
        if (split.length() < 3) {
            log.error("Invalid line in /system/mount_table");
            line = reader.readLine();
            continue;
        }

        auto success = filesystemService.mount(split[0], split[1], split[2]);
        if (!success) {
            log.error("Failed to mount [%s] to [%s]", static_cast<const char*>(split[0]), static_cast<const char*>(split[1]));
        }

        line = reader.readLine();
    }
}
