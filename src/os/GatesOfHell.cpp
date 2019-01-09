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

#include <kernel/cpu/CpuId.h>
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
#include <kernel/Bios.h>
#include <devices/graphics/text/LfbText.h>
#include <kernel/interrupts/BlueScreenLfb.h>
#include "GatesOfHell.h"
#include "BuildConfig.h"

Logger &GatesOfHell::log = Logger::get("BOOT");

ModuleLoader *GatesOfHell::moduleLoader = nullptr;

GraphicsService *GatesOfHell::graphicsService = nullptr;

EventBus *GatesOfHell::eventBus = nullptr;

Bootscreen *GatesOfHell::bootscreen = nullptr;

IdleThread *GatesOfHell::idleThread = nullptr;

uint16_t GatesOfHell::xres = 0;

uint16_t GatesOfHell::yres = 0;

uint8_t GatesOfHell::bpp = 0;

int32_t main() {

    return GatesOfHell::enter();
}

int32_t GatesOfHell::enter() {

    log.trace("Booting hhuOS %s - git %s", BuildConfig::VERSION, BuildConfig::GIT_REV);
    log.trace("Build date: %s", BuildConfig::BUILD_DATE);

    eventBus = new EventBus();

    log.trace("Registering services");

    registerServices();

    log.trace("Initializing BIOS calls");

    Bios::init();

    moduleLoader = Kernel::getService<ModuleLoader>();

    auto *fs = Kernel::getService<FileSystem>();
    fs->mountInitRamdisk("/");

    afterInitrdModHook();

    log.trace("Plugging in RTC");

    auto *rtc = Kernel::getService<TimeService>()->getRTC();
    rtc->plugin();

    log.trace("Plugging in keyboard and mouse");

    auto *inputService = Kernel::getService<InputService>();
    inputService->getKeyboard()->plugin();
    inputService->getMouse()->plugin();

    log.trace("Initializing graphics");

    initializeGraphics();

    bool showSplash = Multiboot::Structure::getKernelOption("splash") == "true";

    bootscreen = new Bootscreen(showSplash, log);

    bootscreen->init(xres, yres, bpp);

    bootscreen->update(0, "Initializing PCI Devices");
    Pci::scan();

    afterPciScanModHook();

    bootscreen->update(33, "Initializing Filesystem");

    fs->init();
    sys_init_libc();

    afterFsInitModHook();

    initializePorts();

    initializeMemoryManagers();

    bootscreen->update(66, "Starting Threads");
    idleThread = new IdleThread();

    idleThread->start();
    eventBus->start();
    InterruptManager::getInstance().start();
    Application::getInstance().start();

    bootscreen->update(100, "Finished Booting!");

    BeepFile *sound = BeepFile::load("/initrd/music/beep/startup.beep");

    if(sound != nullptr) {
        sound->play();
        delete sound;
    }

    bootscreen->finish();

    if (!showSplash) {

        Logger::setConsoleLogging(false);
    }

    Scheduler::getInstance().startUp();

    return 0;
}

void GatesOfHell::registerServices() {

    Kernel::registerService(EventBus::SERVICE_NAME, eventBus);

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
}

void GatesOfHell::afterInitrdModHook() {
    log.trace("Entering after_initrd_mod_hook");

    Util::Array<String> modules = Multiboot::Structure::getKernelOption("after_initrd_mod_hook").split(",");

    for(const auto &module : modules) {
        loadModule("/mod/" + module);
    }

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

    log.trace("Leaving after_fs_init_mod_hook");
}

void GatesOfHell::initializeGraphics() {

    graphicsService = Kernel::getService<GraphicsService>();

    // Check, if a graphics mode has already been set by GRUB
    Multiboot::FrameBufferInfo fbInfo = Multiboot::Structure::getFrameBufferInfo();

    if(fbInfo.address != nullptr) {
        void *virtAddress = fbInfo.address;

        auto *genericLfb = new LinearFrameBuffer(virtAddress, static_cast<uint16_t>(fbInfo.width),
                                                 static_cast<uint16_t>(fbInfo.height), fbInfo.bpp,
                                                 static_cast<uint16_t>(fbInfo.pitch));

        auto *genericTextDriver = new LfbText(virtAddress, static_cast<uint16_t>(fbInfo.width),
                                              static_cast<uint16_t>(fbInfo.height), fbInfo.bpp,
                                              static_cast<uint16_t>(fbInfo.pitch));

        graphicsService->registerLinearFrameBuffer(genericLfb);
        graphicsService->registerTextDriver(genericTextDriver);
    }

    // Get desired graphics driver from GRUB
    String lfbName = Multiboot::Structure::getKernelOption("linear_frame_buffer");
    String textName =  Multiboot::Structure::getKernelOption("text_driver");

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

    moduleLoader->load(file);

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