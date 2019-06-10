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
#include "kernel/core/SystemManagement.h"
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

Logger &GatesOfHell::log = Logger::get("BOOT");

AnsiOutputStream *GatesOfHell::outputStream = nullptr;

BootScreen *GatesOfHell::bootscreen = nullptr;

IdleThread *GatesOfHell::idleThread = nullptr;

uint16_t GatesOfHell::xres = 0;

uint16_t GatesOfHell::yres = 0;

uint8_t GatesOfHell::bpp = 0;

int32_t main() {

    GatesOfHell::enter();
}

BootComponent GatesOfHell::initBiosComponent("InitBiosComponent", Util::Array<BootComponent*>(0), []{

    log.trace("Initializing BIOS calls");

    Bios::init();

    log.trace("Finished initializing BIOS calls");
});

BootComponent GatesOfHell::initServicesComponent("InitServicesComponent", Util::Array<BootComponent*>(0), []{

    log.trace("Registering services");

    registerServices();

    log.trace("Finished registering services");

    Kernel::getService<Filesystem>()->mountInitRamdisk("/");

    afterInitrdModHook();
});

BootComponent GatesOfHell::initGraphicsComponent("InitGraphicsComponent", Util::Array<BootComponent*>({&initServicesComponent}), []{

    log.trace("Initializing graphics");

    initializeGraphics();

    outputStream = new AnsiOutputStream();

    stdout = outputStream;

    Logger::setConsoleLogging(Multiboot::Structure::getKernelOption("splash") != "true");

    log.trace("Finished initializing graphics");

    bootscreen = new BootScreen(coordinator);

    if(Multiboot::Structure::getKernelOption("splash") == "true") {

        bootscreen->init(xres, yres, bpp);
    }
});

BootComponent GatesOfHell::scanPciBusComponent("ScanPciBusComponent", Util::Array<BootComponent*>({&initGraphicsComponent}),[]{

    log.trace("Scanning PCI devices");

    Pci::scan();

    log.trace("Finished scanning PCI devices");

    afterPciScanModHook();
});

BootComponent GatesOfHell::initFilesystemComponent("InitFilesystemComponent", Util::Array<BootComponent*>({&scanPciBusComponent}), []{

    log.trace("Initializing filesystem");

    Kernel::getService<Filesystem>()->init();

    log.trace("Finished initializing filesystem");

    afterFsInitModHook();
});

BootComponent GatesOfHell::initPortsComponent("InitPortsComponent", Util::Array<BootComponent*>({&initFilesystemComponent}), []{

    log.trace ("Initializing ports");

    initializePorts();

    log.trace ("Finished initializing ports");
});

BootComponent GatesOfHell::initMemoryManagersComponent("InitMemoryManagersComponent",  Util::Array<BootComponent*>({&initFilesystemComponent}), []{

    log.trace ("Initializing memory managers");

    initializeMemoryManagers();

    log.trace ("Finished initializing memory managers");
});

BootComponent GatesOfHell::parsePciDatabaseComponent("ParsePciDatabaseComponent", Util::Array<BootComponent*>({&initFilesystemComponent}),[]{

    log.trace ("Parsing PCI database");

    Pci::parseDatabase();

    log.trace ("Finished parsing PCI database");
});

BootCoordinator GatesOfHell::coordinator(Util::Array<BootComponent*>({&initServicesComponent,
        &initGraphicsComponent, &scanPciBusComponent, &initFilesystemComponent, &initPortsComponent,
        &initMemoryManagersComponent}), []{

    Kernel::getService<EventBus>()->start();

    BeepFile *sound = BeepFile::load("/initrd/music/beep/startup.beep");

    if(sound != nullptr) {
        sound->play();
        delete sound;
    }

    stdout = File::open("/dev/stdout", "w");

    bootscreen->finish();

    Logger::setConsoleLogging(false);

    delete outputStream;

    Application::getInstance().start();
});

void GatesOfHell::enter() {

    log.trace("Booting hhuOS %s - git %s", BuildConfig::getVersion(), BuildConfig::getGitRevision());
    log.trace("Build date: %s", BuildConfig::getBuildDate());

    if(Multiboot::Structure::getKernelOption("bios_enhancements") == "true") {
        coordinator.addComponent(&initBiosComponent);
        initServicesComponent.addDependency(&initBiosComponent);
    }

    if (Multiboot::Structure::getKernelOption("pci_names") == "true") {
        coordinator.addComponent(&parsePciDatabaseComponent);
    }

    idleThread = new IdleThread();
    idleThread->start();

    InterruptManager::getInstance().start();

    coordinator.start();

    Scheduler::getInstance().startUp();

    Cpu::halt();
}

void GatesOfHell::registerServices() {

    Kernel::registerService(EventBus::SERVICE_NAME, new EventBus());

    Kernel::registerService(GraphicsService::SERVICE_NAME, new GraphicsService());
    Kernel::registerService(TimeService::SERVICE_NAME, new TimeService(Pit::getInstance()));
    Kernel::registerService(StorageService::SERVICE_NAME, new StorageService());
    Kernel::registerService(Filesystem::SERVICE_NAME, new Filesystem());
    Kernel::registerService(InputService::SERVICE_NAME, new InputService());
    Kernel::registerService(DebugService::SERVICE_NAME, new DebugService());
    Kernel::registerService(ModuleLoader::SERVICE_NAME, new ModuleLoader());
    Kernel::registerService(KernelStreamService::SERVICE_NAME, new KernelStreamService());
    Kernel::registerService(SoundService::SERVICE_NAME, new SoundService());
    Kernel::registerService(PortService::SERVICE_NAME, new PortService());
    Kernel::registerService(ScreenshotService::SERVICE_NAME, new ScreenshotService());
    Kernel::registerService(NetworkService::SERVICE_NAME, new NetworkService());
}

void GatesOfHell::afterInitrdModHook() {
    log.trace("Entering after_initrd_mod_hook");

    Util::Array<String> modules = Multiboot::Structure::getKernelOption("after_initrd_mod_hook").split(",");

    for(const auto &module : modules) {
        loadModule("/mod/" + module);
    }

    auto *cgaLfb = new CgaGraphics();
    if(cgaLfb->isAvailable()) {
        log.info("Detected a CGA compatible graphics card");
        Kernel::getService<GraphicsService>()->registerLinearFrameBuffer(cgaLfb);
        Kernel::getService<GraphicsService>()->registerTextDriver(new CgaText());
    }

    auto *vesaLfb = new VesaGraphics();
    if(cgaLfb->isAvailable()) {
        log.info("Detected a VESA compatible graphics card");
        Kernel::getService<GraphicsService>()->registerLinearFrameBuffer(vesaLfb);
        Kernel::getService<GraphicsService>()->registerTextDriver(new VesaText());
    }

    if(FloppyController::isAvailable()) {
        log.info("Floppy controller is available and at least one drive is attached to it");
        auto *floppyController = new FloppyController();
        floppyController->plugin();
        floppyController->setup();
    }

    if(SoundBlaster::isAvailable()) {
        log.info("Found a SoundBlaster device");

        Kernel::getService<SoundService>()->setPcmAudioDevice(SoundBlaster::initialize());
    } else {
        log.info("No SoundBlaster device available");
    }

    FsDriver::registerPrototype(new FatDriver());

    MemoryManager::registerPrototype(new StaticHeapMemoryManager());

    Kernel::getService<TimeService>()->getRTC()->plugin();

    auto *inputService = Kernel::getService<InputService>();
    inputService->getKeyboard()->plugin();
    inputService->getMouse()->plugin();

    log.trace("Leaving after_initrd_mod_hook");
}

void GatesOfHell::afterPciScanModHook() {
    log.trace("Entering after_pci_scan_mod_hook");

    Util::Array<String> modules = Multiboot::Structure::getKernelOption("after_pci_scan_mod_hook").split(",");

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

    Util::Array<String> modules = Multiboot::Structure::getKernelOption("after_fs_init_mod_hook").split(",");

    for(const auto &module : modules) {
        loadModule("/initrd/mod/" + module);
    }

    auto filesystem = Kernel::getService<Filesystem>();

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

    auto *graphicsService = Kernel::getService<GraphicsService>();

    // Check, if a graphics mode has already been set by GRUB
    Multiboot::FrameBufferInfo fbInfo = Multiboot::Structure::getFrameBufferInfo();

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
    String lfbName = Multiboot::Structure::getKernelOption("linear_frame_buffer");
    String textName =  Multiboot::Structure::getKernelOption("text_driver");

    if(lfbName.isEmpty()) {
        lfbName = "LinearFrameBuffer";
    }

    if(textName.isEmpty()) {
        textName = "LfbText";
    }

    // Get desired resolution from GRUB
    Util::Array<String> res = Multiboot::Structure::getKernelOption("resolution").split("x");

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

        Kernel::getService<KernelStreamService>()->setStdout(graphicsService->getTextDriver());
        Kernel::getService<KernelStreamService>()->setStderr(graphicsService->getTextDriver());

        stdout = graphicsService->getTextDriver();
    }
}

void GatesOfHell::initializeMemoryManagers() {
    MemoryManager::registerPrototype(new FreeListMemoryManager());
    MemoryManager::registerPrototype(new BitmapMemoryManager());
}

bool GatesOfHell::loadModule(const String &path) {
    File *file = File::open(path, "r");

    if(file == nullptr) {
        log.warn("Module not found '%s'", (const char*) path);

        return false;
    }

    log.trace("Loading module '%s'", (const char*) path);

    Kernel::getService<ModuleLoader>()->load(file);

    delete file;

    return true;
}

void GatesOfHell::initializePorts() {
    String gdbPortName = Multiboot::Structure::getKernelOption("gdb");

    if (!gdbPortName.isEmpty()) {
        Port *port = Kernel::getService<PortService>()->getPort(gdbPortName);

        GdbServer::initialize(port);

        log.trace("Waiting for GDB debugger...\n");

        GdbServer::synchronize();
    }

    Util::Array<String> logDevices = Multiboot::Structure::getKernelOption("log_devices").split(",");
    auto *portService = Kernel::getService<PortService>();

    for(const auto &device : logDevices) {
        if(portService->isPortAvailable(device)) {
            Logger::addAppender(new PortAppender(*portService->getPort(device)));
        }
    }
}