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

/**
 * Main function - is called from assembler code. 
 * 
 * @author Michael Schoettner, Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski
 * @date HHU, 2018
 */

#include <lib/file/tar/Archive.h>
#include <kernel/threads/IdleThread.h>
#include "apps/Application.h"
#include "devices/block/storage/AhciDevice.h"
#include <kernel/services/ModuleLoader.h>
#include <devices/graphics/lfb/VesaGraphics.h>
#include <devices/graphics/lfb/CgaGraphics.h>
#include <kernel/services/InputService.h>
#include <kernel/services/StdStreamService.h>
#include <kernel/services/SoundService.h>
#include <devices/graphics/text/VesaText.h>
#include <devices/Pit.h>
#include <kernel/threads/Scheduler.h>
#include <lib/multiboot/Structure.h>
#include <kernel/log/Logger.h>
#include <kernel/services/SerialService.h>
#include <kernel/services/ParallelService.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/debug/GdbServer.h>
#include <kernel/log/SerialAppender.h>
#include <kernel/cpu/CpuId.h>

#include "bootlogo.h"

extern char *gitversion;

extern "C" {
 void set_debug_traps();
 void breakpoint();
 void idt();
 void exceptionHandler(unsigned int number, debugFunction dbgFunc);
}

auto versionString = String("hhuOS ") + String(gitversion);

uint16_t xres = 800;
uint16_t yres = 600;
uint8_t bpp = 32;

IdleThread *idleThread = nullptr;
EventBus *eventBus = nullptr;
LinearFrameBuffer *lfb = nullptr;
TextDriver *text = nullptr;

void updateBootScreen(uint8_t percentage, const char *currentActivity) {
    auto normalizedPercentage = static_cast<uint8_t>((percentage * 60) / 100);

    lfb->fillRect(0, 0, lfb->getResX(), lfb->getResY(), Colors::HHU_DARK_BLUE);

    lfb->placeString(sun_font_8x16, 50, 10, static_cast<char *>(versionString), Colors::HHU_GRAY, Colors::INVISIBLE);

    if(lfb->getResY() < 350) {
        lfb->placeSprite(50, 45, static_cast<uint16_t>(bootlogo_75x75.width),
                         static_cast<uint16_t>(bootlogo_75x75.height), (int32_t *) (&bootlogo_75x75.pixel_data[0]));
    } else {
        lfb->placeSprite(50, 45, static_cast<uint16_t>(bootlogo_200x200.width),
                         static_cast<uint16_t>(bootlogo_200x200.height), (int32_t *) (&bootlogo_200x200.pixel_data[0]));
    }

    lfb->placeFilledRect(20, 80, 60, 2, Colors::HHU_BLUE_30);
    lfb->placeFilledCircle(20, 81, 1, Colors::HHU_BLUE_30);
    lfb->placeFilledCircle(80, 81, 1, Colors::HHU_BLUE_30);

    lfb->placeFilledRect(20, 80, normalizedPercentage, 2, Colors::HHU_BLUE);
    lfb->placeFilledCircle(20, 81, 1, Colors::HHU_BLUE);
    lfb->placeFilledCircle(static_cast<uint16_t>(20 + normalizedPercentage), 81, 1, Colors::HHU_BLUE);

    lfb->placeString(sun_font_8x16, 50, 90, currentActivity, Colors::HHU_GRAY, Colors::INVISIBLE);

    lfb->show();
}

void registerServices() {
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

void initGraphics() {
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

void initSerialPorts() {
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

void loadInitrd() {

    Multiboot::ModuleInfo info = Multiboot::Structure::getModule("initrd");

    Address address(info.start);

    Tar::Archive &archive = Tar::Archive::from(address);

    Util::Array<Tar::Header> header = archive.getFileHeaders();
}

int32_t main() {

    if (CpuId::isSupported()) {

        Logger::trace("Detected CPUID support");
    }

    Logger::trace("Initializing graphics");

    initGraphics();

    eventBus = new EventBus();

    Logger::trace("Registering services");

    registerServices();

    Logger::trace("Initializing serial ports");

    initSerialPorts();

    if (Multiboot::Structure::getKernelOption("gdb") == "true") {

        GdbServer::initialize();

        printf("Waiting for GDB debugger...\n");

        GdbServer::synchronize();
    }

    Logger::trace("Plugging in RTC");

    auto *rtc = Kernel::getService<TimeService>()->getRTC();
    rtc->plugin();

    Logger::trace("Plugging in keyboard and mouse");

    auto *inputService = Kernel::getService<InputService>();
    inputService->getKeyboard()->plugin();
    inputService->getMouse()->plugin();

    if(Multiboot::Structure::getKernelOption("splash") == "true") {
        lfb->init(xres, yres, bpp);
        lfb->enableDoubleBuffering();

        updateBootScreen(0, "Initializing PCI Devices");
        Pci::scan();

        updateBootScreen(33, "Initializing Filesystem");
        auto *fs = Kernel::getService<FileSystem>();
        fs->init();
        printfUpdateStdout();

        updateBootScreen(66, "Starting Threads");
        idleThread = new IdleThread();

        idleThread->start();
        eventBus->start();
        Application::getInstance()->start();

        updateBootScreen(100, "Finished Booting!");
        Kernel::getService<TimeService>()->msleep(1000);
        lfb->disableDoubleBuffering();
        lfb->clear();
    } else {
        text->puts("Initializing PCI Devices\n", 25, Colors::HHU_RED);
        Pci::scan();

        text->puts("Initializing Filesystem\n", 24, Colors::HHU_RED);
        auto *fs = Kernel::getService<FileSystem>();
        fs->init();
        printfUpdateStdout();

        text->puts("Starting Threads\n", 17, Colors::HHU_RED);
        idleThread = new IdleThread();

        idleThread->start();
        eventBus->start();
        Application::getInstance()->start();

        text->puts("\n\nFinished Booting! Please press Enter!\n", 40, Colors::HHU_BLUE);

        while (!inputService->getKeyboard()->isKeyPressed(28));

        lfb->init(xres, yres, bpp);
    }

    Logger::trace("Starting scheduler");

    Scheduler::getInstance()->schedule();

    return 0;
}
