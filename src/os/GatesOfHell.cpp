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
#include <devices/storage/controller/FloppyController.h>
#include <filesystem/TarArchive/TarArchiveNode.h>
#include <filesystem/TarArchive/TarArchiveDriver.h>
#include <lib/file/Directory.h>
#include <lib/file/beep/BeepFile.h>
#include <kernel/services/ScreenshotService.h>
#include <lib/file/wav/Wav.h>
#include <devices/sound/SoundBlaster/SoundBlaster.h>
#include <devices/IODeviceManager.h>
#include <lib/libc/system_interface.h>
#include <lib/file/FileStatus.h>
#include <kernel/memory/manager/FreeListMemoryManager.h>
#include <kernel/memory/SystemManagement.h>
#include <kernel/log/PortAppender.h>
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

    moduleLoader = Kernel::getService<ModuleLoader>();

    auto *fs = Kernel::getService<FileSystem>();
    fs->mountInitRamdisk("/");

    log.trace("Initializing graphics");

    initializeGraphics();

    log.trace("Plugging in RTC");

    auto *rtc = Kernel::getService<TimeService>()->getRTC();
    rtc->plugin();

    log.trace("Plugging in keyboard and mouse");

    auto *inputService = Kernel::getService<InputService>();
    inputService->getKeyboard()->plugin();
    inputService->getMouse()->plugin();

    bool showSplash = Multiboot::Structure::getKernelOption("splash") == "true";

    bootscreen = new Bootscreen(showSplash, log);

    bootscreen->init(xres, yres, bpp);

    bootscreen->update(0, "Initializing ISA Devices");
    if(FloppyController::isAvailable()) {
        log.trace("Floppy controller is available and at least one drive is attached to it");

        auto *floppyController = new FloppyController();
        floppyController->plugin();
        floppyController->setup();
    }

    if(SoundBlaster::isAvailable()) {
        log.info("Found audio device: SoundBlaster");

        Kernel::getService<SoundService>()->setPcmAudioDevice(SoundBlaster::initialize());
    }

    bootscreen->update(25, "Initializing PCI Devices");
    Pci::scan();

    initializePciDrivers();

    bootscreen->update(50, "Initializing Filesystem");
    loadModule("/mod/fat.ko");

    fs->init();
    sys_init_libc();

    initializePorts();

    loadModule("/initrd/mod/zero.ko");
    loadModule("/initrd/mod/random.ko");

    initializeMemoryManagers();

    bootscreen->update(75, "Starting Threads");
    idleThread = new IdleThread();

    idleThread->start();
    eventBus->start();
    IODeviceManager::getInstance().start();
    Application::getInstance().start();

    bootscreen->update(100, "Finished Booting!");

    BeepFile *sound = BeepFile::load("/initrd/music/beep/startup.beep");

    Kernel::getService<GraphicsService>()->getLinearFrameBuffer()->disableDoubleBuffering();

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

void GatesOfHell::initializeGraphics() {

    graphicsService = Kernel::getService<GraphicsService>();

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

    loadModule("/mod/cga.ko");
    loadModule("/mod/vesa.ko");

    graphicsService->setLinearFrameBuffer(lfbName);
    graphicsService->setTextDriver(textName);

    graphicsService->getLinearFrameBuffer()->init(xres, yres, bpp);
    graphicsService->getTextDriver()->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

    Kernel::getService<KernelStreamService>()->setStdout(graphicsService->getTextDriver());
    Kernel::getService<KernelStreamService>()->setStderr(graphicsService->getTextDriver());

    stdout = graphicsService->getTextDriver();
}

void GatesOfHell::initializePciDrivers() {
    Ahci ahci;
    Uhci uhci;

    Pci::setupDeviceDriver(ahci);
    Pci::setupDeviceDriver(uhci);
}

void GatesOfHell::initializeMemoryManagers() {
    MemoryManager::registerPrototype(new FreeListMemoryManager());
    MemoryManager::registerPrototype(new BitmapMemoryManager());

    loadModule("/initrd/mod/static-heap.ko");
}

bool GatesOfHell::loadModule(const String &path) {
    File *file = File::open(path, "r");

    if(file == nullptr) {
        return false;
    }

    moduleLoader->load(file);

    delete file;

    return true;
}

void GatesOfHell::initializePorts() {
    loadModule("/initrd/mod/serial.ko");
    loadModule("/initrd/mod/parallel.ko");

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
