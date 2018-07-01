#include <kernel/cpu/CpuId.h>
#include <kernel/services/EventBus.h>
#include <lib/multiboot/Structure.h>
#include <kernel/debug/GdbServer.h>
#include <lib/libc/printf.h>
#include <kernel/services/TimeService.h>
#include <kernel/services/InputService.h>
#include <devices/Pci.h>
#include <filesystem/FileSystem.h>
#include <kernel/threads/IdleThread.h>
#include <apps/Application.h>
#include <kernel/threads/Scheduler.h>
#include <devices/Pit.h>
#include <kernel/services/DebugService.h>
#include <kernel/services/ModuleLoader.h>
#include <kernel/services/StdStreamService.h>
#include <kernel/services/SoundService.h>
#include <kernel/services/SerialService.h>
#include <kernel/services/ParallelService.h>
#include <devices/graphics/text/VesaText.h>
#include <devices/graphics/lfb/CgaGraphics.h>
#include <lib/file/tar/Archive.h>
#include "GatesOfHell.h"

Logger &GatesOfHell::log = Logger::get("BOOT");

LinearFrameBuffer *GatesOfHell::lfb = nullptr;

TextDriver *GatesOfHell::text = nullptr;

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

    log.trace("Initializing graphics");

    initializeGraphics();

    eventBus = new EventBus();

    log.trace("Registering services");

    registerServices();

    log.trace("Initializing serial ports");

    initializeSerialPorts();

    if (Multiboot::Structure::getKernelOption("gdb") == "true") {

        GdbServer::initialize();

        printf("Waiting for GDB debugger...\n");

        GdbServer::synchronize();
    }

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

    bootscreen->update(0, "Initializing PCI Devices");
    Pci::scan();

    bootscreen->update(33, "Initializing Filesystem");
    auto *fs = Kernel::getService<FileSystem>();
    fs->init();
    printfUpdateStdout();

    bootscreen->update(66, "Starting Threads");
    idleThread = new IdleThread();

    idleThread->start();
    eventBus->start();
    Application::getInstance()->start();

    bootscreen->update(100, "Finished Booting!");
    Kernel::getService<TimeService>()->msleep(1000);

    bootscreen->finish();

    Scheduler::getInstance()->schedule();

    return 0;
}

void GatesOfHell::registerServices() {

    Kernel::registerService(EventBus::SERVICE_NAME, eventBus);

    auto *graphicsService = new GraphicsService();
    graphicsService->setLinearFrameBuffer(lfb);
    graphicsService->setTextDriver(text);

    Kernel::registerService(GraphicsService::SERVICE_NAME, graphicsService);
    Kernel::registerService(TimeService::SERVICE_NAME, new TimeService(Pit::getInstance()));
    Kernel::registerService(StorageService::SERVICE_NAME, new StorageService());
    Kernel::registerService(FileSystem::SERVICE_NAME, new FileSystem());
    Kernel::registerService(InputService::SERVICE_NAME, new InputService());
    Kernel::registerService(DebugService::SERVICE_NAME, new DebugService());
    Kernel::registerService(ModuleLoader::SERVICE_NAME, new ModuleLoader());
    Kernel::registerService(StdStreamService::SERVICE_NAME, new StdStreamService());
    Kernel::registerService(SoundService::SERVICE_NAME, new SoundService());
    Kernel::registerService(SerialService::SERVICE_NAME, new SerialService());
    Kernel::registerService(ParallelService::SERVICE_NAME, new ParallelService());

    Kernel::getService<StdStreamService>()->setStdout(text);
    Kernel::getService<StdStreamService>()->setStderr(text);
}

void GatesOfHell::initializeGraphics() {

    auto *vesa = new VesaGraphics();

    // Get desired resolution from GRUB
    Util::Array<String> res = Multiboot::Structure::getKernelOption("vbe").split("x");

    if(res.length() >= 3) {
        xres = static_cast<uint16_t>(strtoint((const char *) res[0]));
        yres = static_cast<uint16_t>(strtoint((const char *) res[1]));
        bpp = static_cast<uint8_t>(strtoint((const char *) res[2]));
    }

    // Detect video capability
    if(vesa->isAvailable()) {
        lfb = vesa;
        text = new VesaText();
    } else {
        delete vesa;
        auto *cga = new CgaGraphics();
        if(cga->isAvailable()) {
            lfb = cga;
            text = new CgaText();
        } else {
            //No VBE and no CGA? Your machine is waaaaay to old...
            delete cga;
            Cpu::halt();
        }
    }

    // Initialize drivers
    lfb->init(xres, yres, bpp);
    text->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

    stdout = text;
    text->setpos(0, 0);
}

void GatesOfHell::initializeSerialPorts() {

    auto *serialService = Kernel::getService<SerialService>();

    if(serialService->isPortAvailable(Serial::COM1)) {
        serialService->getSerialPort(Serial::COM1)->plugin();
    }

    if(serialService->isPortAvailable(Serial::COM2)) {
        serialService->getSerialPort(Serial::COM2)->plugin();
    }

    if(serialService->isPortAvailable(Serial::COM3)) {
        serialService->getSerialPort(Serial::COM3)->plugin();
    }

    if(serialService->isPortAvailable(Serial::COM4)) {
        serialService->getSerialPort(Serial::COM4)->plugin();
    }
}

void GatesOfHell::loadInitrd() {

    Multiboot::ModuleInfo info = Multiboot::Structure::getModule("initrd");

    Address address(info.start);

    Tar::Archive &archive = Tar::Archive::from(address);

    Util::Array<Tar::Header> header = archive.getFileHeaders();
}
