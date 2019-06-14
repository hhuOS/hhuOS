/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include <filesystem/ram/nodes/memory/KernelHeapNode.h>
#include <filesystem/ram/nodes/memory/IoMemoryNode.h>
#include <filesystem/ram/nodes/memory/PhysicalMemoryNode.h>
#include <filesystem/ram/nodes/memory/PagingAreaNode.h>
#include <filesystem/ram/nodes/util/RandomNode.h>
#include <filesystem/ram/nodes/util/ZeroNode.h>
#include <filesystem/ram/nodes/video/GraphicsVendorNameNode.h>
#include <filesystem/ram/nodes/video/GraphicsDeviceNameNode.h>
#include <filesystem/ram/nodes/video/GraphicsMemoryNode.h>
#include <filesystem/ram/nodes/video/GraphicsResolutionsNode.h>
#include <filesystem/ram/nodes/video/CurrentResolutionNode.h>
#include <filesystem/ram/nodes/video/CurrentGraphicsDriverNode.h>
#include <filesystem/ram/nodes/video/GraphicsDriversNode.h>
#include "kernel/memory/manager/StaticHeapMemoryManager.h"
#include "filesystem/fat/FatDriver.h"
#include "device/graphic/vesa/VesaGraphics.h"
#include "device/graphic/vesa/VesaText.h"
#include "device/sound/soundblaster/SoundBlaster.h"
#include "device/graphic/cga/CgaGraphics.h"
#include "device/graphic/cga/CgaText.h"
#include "device/storage/floppy/FloppyController.h"
#include "device/cpu/CpuId.h"
#include "kernel/service/EventBus.h"
#include "kernel/multiboot/Structure.h"
#include "kernel/debug/GdbServer.h"
#include "lib/libc/printf.h"
#include "kernel/service/TimeService.h"
#include "kernel/service/InputService.h"
#include "device/pci/Pci.h"
#include "filesystem/core/Filesystem.h"
#include "application/Application.h"
#include "kernel/thread/Scheduler.h"
#include "device/time/Pit.h"
#include "kernel/service/DebugService.h"
#include "kernel/service/ModuleLoader.h"
#include "kernel/service/KernelStreamService.h"
#include "kernel/service/SoundService.h"
#include "kernel/service/PortService.h"
#include "device/storage/ahci/AhciController.h"
#include "device/usb/Uhci.h"
#include "filesystem/tar/TarArchiveNode.h"
#include "filesystem/tar/TarArchiveDriver.h"
#include "lib/file/Directory.h"
#include "lib/file/beep/BeepFile.h"
#include "kernel/service/ScreenshotService.h"
#include "lib/file/wav/Wav.h"
#include "kernel/interrupt/InterruptManager.h"
#include "lib/libc/system_interface.h"
#include "lib/file/FileStatus.h"
#include "kernel/memory/manager/FreeListMemoryManager.h"
#include "kernel/core/Management.h"
#include "kernel/log/PortAppender.h"
#include "device/misc/Bios.h"
#include "device/graphic/text/LfbText.h"
#include "kernel/bluescreen/BlueScreenLfb.h"
#include "application/mouse/MouseApp.h"
#include "device/misc/Cmos.h"
#include "device/network/e1000/driver/intel82541IP/Intel82541IP.h"
#include "device/network/e1000/driver/intel82540EM/Intel82540EM.h"
#include "kernel/service/NetworkService.h"
#include "application/shell/Shell.h"
#include "device/port/parallel/ParallelDriver.h"
#include "device/port/serial/SerialDriver.h"
#include "GatesOfHell.h"
#include "BuildConfig.h"

Kernel::Logger &GatesOfHell::log = Kernel::Logger::get("BOOT");

AnsiOutputStream *GatesOfHell::outputStream = nullptr;

Kernel::BootScreen *GatesOfHell::bootscreen = nullptr;

Kernel::IdleThread *GatesOfHell::idleThread = nullptr;

uint16_t GatesOfHell::xres = 0;

uint16_t GatesOfHell::yres = 0;

uint8_t GatesOfHell::bpp = 0;

int32_t main() {

    GatesOfHell::enter();
}

Kernel::BootComponent GatesOfHell::initBiosComponent("InitBiosComponent", Util::Array<Kernel::BootComponent*>(0), []{

    log.trace("Initializing BIOS calls");

    Bios::init();

    log.trace("Finished initializing BIOS calls");
});

Kernel::BootComponent GatesOfHell::initServicesComponent("InitServicesComponent", Util::Array<Kernel::BootComponent*>(0), []{

    log.trace("Registering services");

    registerServices();

    log.trace("Finished registering services");

    Kernel::System::getService<Filesystem>()->mountInitRamdisk("/");

    afterInitrdModHook();
});

Kernel::BootComponent GatesOfHell::initGraphicsComponent("InitGraphicsComponent", Util::Array<Kernel::BootComponent*>({&initServicesComponent}), []{

    log.trace("Initializing graphics");

    initializeGraphics();

    outputStream = new AnsiOutputStream();

    stdout = outputStream;

    Kernel::Logger::setConsoleLogging(Kernel::Multiboot::Structure::getKernelOption("splash") != "true");

    log.trace("Finished initializing graphics");

    bootscreen = new Kernel::BootScreen(coordinator);

    if(Kernel::Multiboot::Structure::getKernelOption("splash") == "true") {

        bootscreen->init(xres, yres, bpp);
    }
});

Kernel::BootComponent GatesOfHell::scanPciBusComponent("ScanPciBusComponent", Util::Array<Kernel::BootComponent*>({&initGraphicsComponent}),[]{

    log.trace("Scanning PCI devices");

    Pci::scan();

    log.trace("Finished scanning PCI devices");

    afterPciScanModHook();
});

Kernel::BootComponent GatesOfHell::initFilesystemComponent("InitFilesystemComponent", Util::Array<Kernel::BootComponent*>({&scanPciBusComponent}), []{

    log.trace("Initializing filesystem");

    Kernel::System::getService<Filesystem>()->init();

    log.trace("Finished initializing filesystem");

    afterFsInitModHook();
});

Kernel::BootComponent GatesOfHell::initPortsComponent("InitPortsComponent", Util::Array<Kernel::BootComponent*>({&initFilesystemComponent}), []{

    log.trace ("Initializing ports");

    initializePorts();

    log.trace ("Finished initializing ports");
});

Kernel::BootComponent GatesOfHell::initMemoryManagersComponent("InitMemoryManagersComponent",  Util::Array<Kernel::BootComponent*>({&initFilesystemComponent}), []{

    log.trace ("Initializing memory managers");

    initializeMemoryManagers();

    log.trace ("Finished initializing memory managers");
});

Kernel::BootComponent GatesOfHell::parsePciDatabaseComponent("ParsePciDatabaseComponent", Util::Array<Kernel::BootComponent*>({&initFilesystemComponent}),[]{

    log.trace ("Parsing PCI database");

    Pci::parseDatabase();

    log.trace ("Finished parsing PCI database");
});

Kernel::BootCoordinator GatesOfHell::coordinator(Util::Array<Kernel::BootComponent*>({&initServicesComponent,
        &initGraphicsComponent, &scanPciBusComponent, &initFilesystemComponent, &initPortsComponent,
        &initMemoryManagersComponent}), []{

    Kernel::System::getService<Kernel::EventBus>()->start();

    BeepFile *sound = BeepFile::load("/initrd/music/beep/startup.beep");

    if(sound != nullptr) {
        sound->play();
        delete sound;
    }

    stdout = File::open("/dev/stdout", "w");

    bootscreen->finish();

    Kernel::Logger::setConsoleLogging(false);

    delete outputStream;

    Application::getInstance().start();
});

void GatesOfHell::enter() {

    log.trace("Booting hhuOS %s - git %s", BuildConfig::getVersion(), BuildConfig::getGitRevision());
    log.trace("Build date: %s", BuildConfig::getBuildDate());

    Kernel::SystemCall::registerSystemCall(Standard::System::Call::SYSTEM_CALL_TEST, [](uint32_t paramCount, va_list params, Standard::System::Result &result) {
        log.debug("System call with %d parameters!", paramCount);

        uint32_t sum = 0;
        
        for(uint32_t i = 0; i < paramCount; i++) {
            uint32_t param = va_arg(params, uint32_t);
            log.debug("Parameter %d: %d", i, param);
            sum += param;
        }

        result.setValue(sum);
    });

    Standard::System::Result result{};
    Standard::System::Call::execute(Standard::System::Call::Code::SYSTEM_CALL_TEST, result, 3, 10, 20, 30);
    
    log.debug("System call returned with value: %d", result.getValue());

    if(Kernel::Multiboot::Structure::getKernelOption("bios_enhancements") == "true") {
        coordinator.addComponent(&initBiosComponent);
        initServicesComponent.addDependency(&initBiosComponent);
    }

    if (Kernel::Multiboot::Structure::getKernelOption("pci_names") == "true") {
        coordinator.addComponent(&parsePciDatabaseComponent);
    }

    idleThread = new Kernel::IdleThread();
    idleThread->start();

    Kernel::InterruptManager::getInstance().start();

    coordinator.start();

    Kernel::Scheduler::getInstance().startUp();

    Cpu::halt();
}

void GatesOfHell::registerServices() {

    Kernel::System::registerService(Kernel::EventBus::SERVICE_NAME, new Kernel::EventBus());

    Kernel::System::registerService(Kernel::GraphicsService::SERVICE_NAME, new Kernel::GraphicsService());
    Kernel::System::registerService(Kernel::TimeService::SERVICE_NAME, new Kernel::TimeService(Pit::getInstance()));
    Kernel::System::registerService(Kernel::StorageService::SERVICE_NAME, new Kernel::StorageService());
    Kernel::System::registerService(Filesystem::SERVICE_NAME, new Filesystem());
    Kernel::System::registerService(Kernel::InputService::SERVICE_NAME, new Kernel::InputService());
    Kernel::System::registerService(Kernel::DebugService::SERVICE_NAME, new Kernel::DebugService());
    Kernel::System::registerService(Kernel::ModuleLoader::SERVICE_NAME, new Kernel::ModuleLoader());
    Kernel::System::registerService(Kernel::KernelStreamService::SERVICE_NAME, new Kernel::KernelStreamService());
    Kernel::System::registerService(Kernel::SoundService::SERVICE_NAME, new Kernel::SoundService());
    Kernel::System::registerService(Kernel::PortService::SERVICE_NAME, new Kernel::PortService());
    Kernel::System::registerService(Kernel::ScreenshotService::SERVICE_NAME, new Kernel::ScreenshotService());
    Kernel::System::registerService(Kernel::NetworkService::SERVICE_NAME, new Kernel::NetworkService());
}

void GatesOfHell::afterInitrdModHook() {
    log.trace("Entering after_initrd_mod_hook");

    Util::Array<String> modules = Kernel::Multiboot::Structure::getKernelOption("after_initrd_mod_hook").split(",");

    for(const auto &module : modules) {
        loadModule("/mod/" + module);
    }

    auto *cgaLfb = new CgaGraphics();
    if(cgaLfb->isAvailable()) {
        log.info("Detected a CGA compatible graphics card");
        Kernel::System::getService<Kernel::GraphicsService>()->registerLinearFrameBuffer(cgaLfb);
        Kernel::System::getService<Kernel::GraphicsService>()->registerTextDriver(new CgaText());
    }

    auto *vesaLfb = new VesaGraphics();
    if(cgaLfb->isAvailable()) {
        log.info("Detected a VESA compatible graphics card");
        Kernel::System::getService<Kernel::GraphicsService>()->registerLinearFrameBuffer(vesaLfb);
        Kernel::System::getService<Kernel::GraphicsService>()->registerTextDriver(new VesaText());
    }

    if(FloppyController::isAvailable()) {
        log.info("Floppy controller is available and at least one drive is attached to it");
        auto *floppyController = new FloppyController();
        floppyController->plugin();
        floppyController->setup();
    }

    if(SoundBlaster::isAvailable()) {
        log.info("Found a SoundBlaster device");

        Kernel::System::getService<Kernel::SoundService>()->setPcmAudioDevice(SoundBlaster::initialize());
    } else {
        log.info("No SoundBlaster device available");
    }

    FsDriver::registerPrototype(new FatDriver());

    Kernel::MemoryManager::registerPrototype(new Kernel::StaticHeapMemoryManager());

    Kernel::System::getService<Kernel::TimeService>()->getRTC()->plugin();

    auto *inputService = Kernel::System::getService<Kernel::InputService>();
    inputService->getKeyboard()->plugin();
    inputService->getMouse()->plugin();

    log.trace("Leaving after_initrd_mod_hook");
}

void GatesOfHell::afterPciScanModHook() {
    log.trace("Entering after_pci_scan_mod_hook");

    Util::Array<String> modules = Kernel::Multiboot::Structure::getKernelOption("after_pci_scan_mod_hook").split(",");

    for(const auto &module : modules) {
        loadModule("/mod/" + module);
    }

    AhciController ahci;
    Uhci uhci;

    Pci::setupDeviceDriver(ahci);
    Pci::setupDeviceDriver(uhci);

    log.trace("Leaving after_pci_scan_mod_hook");
}

void GatesOfHell::afterFsInitModHook() {
    log.trace("Entering after_fs_init_mod_hook");

    Util::Array<String> modules = Kernel::Multiboot::Structure::getKernelOption("after_fs_init_mod_hook").split(",");

    for(const auto &module : modules) {
        loadModule("/initrd/mod/" + module);
    }

    auto filesystem = Kernel::System::getService<Filesystem>();

    filesystem->createDirectory("/dev/memory");
    filesystem->addVirtualNode("/dev/memory/", new KernelHeapNode());
    filesystem->addVirtualNode("/dev/memory/", new IoMemoryNode());
    filesystem->addVirtualNode("/dev/memory/", new PhysicalMemoryNode());
    filesystem->addVirtualNode("/dev/memory/", new PagingAreaNode());

    filesystem->createDirectory("/dev/util");
    filesystem->addVirtualNode("/dev/util/", new RandomNode());
    filesystem->addVirtualNode("/dev/util/", new ZeroNode());

    filesystem->createDirectory("/dev/video");
    filesystem->createDirectory("/dev/video/text");
    filesystem->createDirectory("/dev/video/lfb");

    filesystem->addVirtualNode("/dev/video/text", new GraphicsVendorNameNode(GraphicsNode::TEXT));
    filesystem->addVirtualNode("/dev/video/text", new GraphicsDeviceNameNode(GraphicsNode::TEXT));
    filesystem->addVirtualNode("/dev/video/text", new GraphicsMemoryNode(GraphicsNode::TEXT));
    filesystem->addVirtualNode("/dev/video/text", new GraphicsResolutionsNode(GraphicsNode::TEXT));
    filesystem->addVirtualNode("/dev/video/text", new CurrentResolutionNode(GraphicsNode::TEXT));
    filesystem->addVirtualNode("/dev/video/text", new GraphicsDriversNode(GraphicsNode::TEXT));
    filesystem->addVirtualNode("/dev/video/text", new CurrentGraphicsDriverNode(GraphicsNode::TEXT));
    filesystem->addVirtualNode("/dev/video/lfb", new GraphicsVendorNameNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    filesystem->addVirtualNode("/dev/video/lfb", new GraphicsDeviceNameNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    filesystem->addVirtualNode("/dev/video/lfb", new GraphicsMemoryNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    filesystem->addVirtualNode("/dev/video/lfb", new GraphicsResolutionsNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    filesystem->addVirtualNode("/dev/video/lfb", new CurrentResolutionNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    filesystem->addVirtualNode("/dev/video/lfb", new GraphicsDriversNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    filesystem->addVirtualNode("/dev/video/lfb", new CurrentGraphicsDriverNode(GraphicsNode::LINEAR_FRAME_BUFFER));

    Serial::initializePorts();
    Parallel::initializePorts();

    Intel82540EM intel82540EM;
    Intel82541IP intel82541IP;

    Pci::setupDeviceDriver(intel82540EM);
    Pci::setupDeviceDriver(intel82541IP);

    log.trace("Leaving after_fs_init_mod_hook");
}

void GatesOfHell::initializeGraphics() {

    auto *graphicsService = Kernel::System::getService<Kernel::GraphicsService>();

    // Check, if a graphics mode has already been set by GRUB
    Kernel::Multiboot::FrameBufferInfo fbInfo = Kernel::Multiboot::Structure::getFrameBufferInfo();

    if(fbInfo.address != nullptr) {
        auto *genericLfb = new LinearFrameBuffer(fbInfo.address, static_cast<uint16_t>(fbInfo.width),
                                                 static_cast<uint16_t>(fbInfo.height), fbInfo.bpp,
                                                 static_cast<uint16_t>(fbInfo.pitch));

        auto *genericTextDriver = new LfbText(fbInfo.address, static_cast<uint16_t>(fbInfo.width),
                                              static_cast<uint16_t>(fbInfo.height), fbInfo.bpp,
                                              static_cast<uint16_t>(fbInfo.pitch));

        graphicsService->registerLinearFrameBuffer(genericLfb);
        graphicsService->registerTextDriver(genericTextDriver);
    }

    // Get desired graphics driver from GRUB
    String lfbName = Kernel::Multiboot::Structure::getKernelOption("linear_frame_buffer");
    String textName =  Kernel::Multiboot::Structure::getKernelOption("text_driver");

    if(lfbName.isEmpty()) {
        lfbName = "LinearFrameBuffer";
    }

    if(textName.isEmpty()) {
        textName = "LfbText";
    }

    // Get desired resolution from GRUB
    Util::Array<String> res = Kernel::Multiboot::Structure::getKernelOption("resolution").split("x");

    if(res.length() >= 3) {
        xres = static_cast<uint16_t>(strtoint((const char *) res[0]));
        yres = static_cast<uint16_t>(strtoint((const char *) res[1]));
        bpp = static_cast<uint8_t>(strtoint((const char *) res[2]));
    }

    graphicsService->setLinearFrameBuffer(lfbName);
    graphicsService->setTextDriver(textName);

    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();
    TextDriver *text = graphicsService->getTextDriver();

    if(lfb != nullptr) {
        graphicsService->getLinearFrameBuffer()->init(xres, yres, bpp);
    }

    if(text != nullptr) {
        graphicsService->getTextDriver()->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

        Kernel::System::getService<Kernel::KernelStreamService>()->setStdout(graphicsService->getTextDriver());
        Kernel::System::getService<Kernel::KernelStreamService>()->setStderr(graphicsService->getTextDriver());

        stdout = graphicsService->getTextDriver();
    }
}

void GatesOfHell::initializeMemoryManagers() {
    Kernel::MemoryManager::registerPrototype(new Kernel::FreeListMemoryManager());
    Kernel::MemoryManager::registerPrototype(new Kernel::BitmapMemoryManager());
}

bool GatesOfHell::loadModule(const String &path) {
    File *file = File::open(path, "r");

    if(file == nullptr) {
        log.warn("Module not found '%s'", (const char*) path);

        return false;
    }

    log.trace("Loading module '%s'", (const char*) path);

    Kernel::System::getService<Kernel::ModuleLoader>()->load(file);

    delete file;

    return true;
}

void GatesOfHell::initializePorts() {
    String gdbPortName = Kernel::Multiboot::Structure::getKernelOption("gdb");

    if (!gdbPortName.isEmpty()) {
        Port *port = Kernel::System::getService<Kernel::PortService>()->getPort(gdbPortName);

        Kernel::GdbServer::initialize(port);

        log.trace("Waiting for GDB debugger...\n");

        Kernel::GdbServer::synchronize();
    }

    Util::Array<String> logDevices = Kernel::Multiboot::Structure::getKernelOption("log_devices").split(",");
    auto *portService = Kernel::System::getService<Kernel::PortService>();

    for(const auto &device : logDevices) {
        if(portService->isPortAvailable(device)) {
            Kernel::Logger::addAppender(new Kernel::PortAppender(*portService->getPort(device)));
        }
    }
}