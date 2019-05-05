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

#include <devices/cpu/CpuId.h>
#include <kernel/services/EventBus.h>
#include <lib/multiboot/Structure.h>
#include <kernel/debug/GdbServer.h>
#include <lib/libc/printf.h>
#include <kernel/services/TimeService.h>
#include <kernel/services/InputService.h>
#include <devices/pci/Pci.h>
#include <filesystem/FileSystem.h>
#include <apps/Application.h>
#include <kernel/threads/Scheduler.h>
#include <devices/timer/Pit.h>
#include <kernel/services/DebugService.h>
#include <kernel/services/ModuleLoader.h>
#include <kernel/services/KernelStreamService.h>
#include <kernel/services/SoundService.h>
#include <kernel/services/PortService.h>
#include <devices/storage/controller/Ahci.h>
#include <devices/usb/Uhci.h>
#include <filesystem/TarArchive/TarArchiveNode.h>
#include <filesystem/TarArchive/TarArchiveDriver.h>
#include <lib/file/Directory.h>
#include <lib/file/beep/BeepFile.h>
#include <kernel/services/ScreenshotService.h>
#include <lib/file/wav/Wav.h>
#include <kernel/interrupts/InterruptManager.h>
#include <lib/libc/system_interface.h>
#include <lib/file/FileStatus.h>
#include <kernel/memory/manager/FreeListMemoryManager.h>
#include <kernel/memory/SystemManagement.h>
#include <kernel/log/PortAppender.h>
#include <devices/misc/Bios.h>
#include <devices/graphics/text/LfbText.h>
#include <kernel/bluescreen/BlueScreenLfb.h>
#include <apps/MouseApp/MouseApp.h>
#include <devices/misc/Cmos.h>
#include <devices/network/e1000/driver/intel82541IP/Intel82541IP.h>
#include <devices/network/e1000/driver/intel82540EM/Intel82540EM.h>
#include <kernel/services/NetworkService.h>
#include "GatesOfHell.h"
#include "BuildConfig.h"

Logger &GatesOfHell::log = Logger::get("BOOT");

Bootscreen *GatesOfHell::bootscreen = nullptr;

IdleThread *GatesOfHell::idleThread = nullptr;

uint16_t GatesOfHell::xres = 0;

uint16_t GatesOfHell::yres = 0;

uint8_t GatesOfHell::bpp = 0;

int32_t main() {

    return GatesOfHell::enter();
}

SimpleThread GatesOfHell::initServicesThread([]{

    log.trace("Registering services");

    registerServices();

    Kernel::getService<EventBus>()->start();

    log.trace("Finished registering services");

    Kernel::getService<FileSystem>()->mountInitRamdisk("/");

    afterInitrdModHook();
});

SimpleThread GatesOfHell::initGraphicsThread([]{

    log.trace("Initializing graphics");

    initializeGraphics();

    log.trace("Finished initializing graphics");

    bool showSplash = Multiboot::Structure::getKernelOption("splash") == "true";

    bootscreen = new Bootscreen(showSplash, log);

    bootscreen->init(xres, yres, bpp);

    bootscreen->update(0, "Booting...");
});

SimpleThread GatesOfHell::scanPciBusThread([]{

    log.trace("Scanning PCI devices");

    Pci::scan();

    log.trace("Finished scanning PCI devices");

    afterPciScanModHook();
});

SimpleThread GatesOfHell::initFilesystemThread([]{

    log.trace("Initializing filesystem");

    Kernel::getService<FileSystem>()->init();

    sys_init_libc();

    log.trace("Finished initializing filesystem");

    afterFsInitModHook();
});

SimpleThread GatesOfHell::initPortsThread([]{

    log.trace ("Initializing ports");

    initializePorts();

    log.trace ("Finished initializing ports");
});

SimpleThread GatesOfHell::initMemoryManagersThread([]{

    log.trace ("Initializing memory managers");

    initializeMemoryManagers();

    log.trace ("Finished initializing memory managers");
});

SimpleThread GatesOfHell::parsePciDatabaseThread([]{

    if (Multiboot::Structure::getKernelOption("pci_names") == "true") {

        log.trace ("Parsing PCI database");

        Pci::parseDatabase();

        log.trace ("Finished parsing PCI database");
    }
});

int32_t GatesOfHell::enter() {

    log.trace("Booting hhuOS %s - git %s", BuildConfig::VERSION, BuildConfig::GIT_REV);
    log.trace("Build date: %s", BuildConfig::BUILD_DATE);

    log.trace("Initializing BIOS calls");

    Bios::init();

    log.trace("Finished initializing BIOS calls");

    idleThread = new IdleThread();
    idleThread->start();

    SimpleThread bootMainThread([] {
        InterruptManager::getInstance().start();

        initServicesThread.start();
        initServicesThread.join();

        initGraphicsThread.start();
        initGraphicsThread.join();

        bootscreen->update(0, "Initializing PCI devices");

        scanPciBusThread.start();
        scanPciBusThread.join();

        bootscreen->update(33, "Initializing Filesystem");

        initFilesystemThread.start();
        initFilesystemThread.join();

        bootscreen->update(66, "Initializing System");

        parsePciDatabaseThread.start();
        initPortsThread.start();
        initMemoryManagersThread.start();

        initPortsThread.join();
        initMemoryManagersThread.join();
        parsePciDatabaseThread.join();

        bootscreen->update(100, "Finished Booting!");

        BeepFile *sound = BeepFile::load("/initrd/music/beep/startup.beep");

        if(sound != nullptr) {
            sound->play();
            delete sound;
        }

        bootscreen->finish();

        Logger::setConsoleLogging(false);

        Application::getInstance().start();
    });

    bootMainThread.start();

    Scheduler::getInstance().startUp();

    return 0;
}

void GatesOfHell::registerServices() {

    Kernel::registerService(EventBus::SERVICE_NAME, new EventBus());

    Kernel::registerService(GraphicsService::SERVICE_NAME, new GraphicsService());
    Kernel::registerService(TimeService::SERVICE_NAME, new TimeService(Pit::getInstance()));
    Kernel::registerService(StorageService::SERVICE_NAME, new StorageService());
    Kernel::registerService(FileSystem::SERVICE_NAME, new FileSystem());
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

    Ahci ahci;
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