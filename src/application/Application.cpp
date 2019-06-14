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

#include "application/bug/BugDefender.h"
#include "device/time/Pit.h"
#include "lib/libc/printf.h"
#include "application/mouse/MouseApp.h"
#include "kernel/service/DebugService.h"
#include "application/shell/Shell.h"
#include "kernel/multiboot/Structure.h"
#include "kernel/thread/Scheduler.h"
#include "lib/async/WorkerThread.h"
#include "application/loop/LoopsAndSound.h"
#include "application/mandelbrot/Mandelbrot.h"
#include "application/memory/MemoryManagerTest.h"
#include "kernel/memory/manager/FreeListMemoryManager.h"
#include "kernel/memory/manager/BitmapMemoryManager.h"
#include "kernel/memory/manager/BitmapMemoryManager.h"
#include "kernel/core/Management.h"
#include "kernel/core/SystemCall.h"
#include "application/memory/MemoryManagerDemo.h"
#include "lib/async/SimpleThread.h"
#include "application/loop/Loop.h"
#include "application/loop/Sound.h"
#include "application/ant/AntApp.h"
#include "lib/libc/snprintf.h"
#include "Application.h"


#define TEST_THREADING 0

Kernel::Thread *currentApp = nullptr;

uint32_t threadSum = 0;

uint32_t worker(const uint32_t &number) {

    return number * 2;
}

void callback(const Kernel::Thread &thread, const uint32_t &number) {

    threadSum += number;
}

Application::Application () : Thread ("Menu") {
    graphicsService = Kernel::System::getService<Kernel::GraphicsService>();
	timeService = Kernel::System::getService<Kernel::TimeService>();
}

Application& Application::getInstance() noexcept {

    static Application instance;

    return instance;
}

void Application::startLoopSoundDemo() {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

    currentApp = new LoopsAndSound();
    currentApp->start();
}

void Application::startAntDemo() {
    currentApp = new AntApp();

    currentApp->start();
}

void Application::startMandelbrotDemo() {

    currentApp = new Mandelbrot();

    currentApp->start();
}

void Application::startMemoryManagerDemo() {

    graphicsService->getLinearFrameBuffer()->init(xres, yres, bpp);

    // Don't use High-Res mode on CGA, as it looks bad.
    if(graphicsService->getLinearFrameBuffer()->getDepth() == 1) {
        graphicsService->getLinearFrameBuffer()->init(320, 200, 2);
    }

    graphicsService->getLinearFrameBuffer()->enableDoubleBuffering();

    currentApp = new MemoryManagerDemo();

    currentApp->start();
}

void Application::startMouseApp() {
    graphicsService->getLinearFrameBuffer()->init(640, 480, 16);

    // Don't use High-Res mode on CGA, as it looks bad.
    if(graphicsService->getLinearFrameBuffer()->getDepth() == 1) {
        graphicsService->getLinearFrameBuffer()->init(320, 200, 2);
    }

    graphicsService->getLinearFrameBuffer()->enableDoubleBuffering();

    currentApp = new MouseApp();

    currentApp->start();
}

void Application::startExceptionDemo() {
    Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "This ist just a test. Please restart your computer!");
}

void Application::startShell() {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

    currentApp = new Shell();

    currentApp->start();
}

void Application::showMenu () {
    Kernel::MemoryManager *kernelHeapManager = Kernel::Management::getKernelHeapManager();
    Kernel::MemoryManager *pageFrameAllocator = Kernel::Management::getInstance().getPageFrameAllocator();
    Kernel::MemoryManager *ioMemoryManager = Kernel::Management::getInstance().getIOMemoryManager();

    Kernel::Scheduler &scheduler = Kernel::Scheduler::getInstance();

    uint32_t kernelMemory = kernelHeapManager->getEndAddress() - kernelHeapManager->getStartAddress();
    uint32_t physicalMemory = pageFrameAllocator->getEndAddress() - pageFrameAllocator->getStartAddress();
    uint32_t ioMemory = ioMemoryManager->getEndAddress() - ioMemoryManager->getStartAddress();

    while(true) {

        LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

        Font &font = lfb->getResY() < 400 ? (Font&) std_font_8x8 : (Font&) sun_font_8x16;

        if(isRunning) {
            uint32_t usedKernelMemory = kernelMemory - kernelHeapManager->getFreeMemory();
            uint32_t usedPhysicalMemory = physicalMemory - pageFrameAllocator->getFreeMemory();
            uint32_t usedIoMemory = ioMemory - ioMemoryManager->getFreeMemory();

            Rtc::Date date = timeService->getRTC()->getCurrentDate();

            lfb->placeRect(50, 50, 98, 98, Colors::HHU_LIGHT_GRAY);

            lfb->placeString(font, 20, 4, (const char*) String::format("Kernel: %u/%u KiB", usedKernelMemory / 1024, kernelMemory / 1024), Colors::HHU_LIGHT_GRAY);

            lfb->placeString(font, 50, 4, (const char*) String::format("Physical: %u/%u KiB", usedPhysicalMemory / 1024, physicalMemory / 1024), Colors::HHU_LIGHT_GRAY);

            lfb->placeString(font, 80, 4, (const char*) String::format("IO: %u/%u KiB", usedIoMemory / 1024, ioMemory / 1024), Colors::HHU_LIGHT_GRAY);

            lfb->drawLine(0, static_cast<uint16_t>(lfb->getResX() - 1), font.get_char_height(), font.get_char_height(), Colors::HHU_BLUE_30);

            lfb->placeString(font, 50, 8, (const char *) String::format("Threads: %u", scheduler.getThreadCount()), Colors::HHU_LIGHT_GRAY);

            lfb->placeString(font, 50, 14, (const char *) String::format("%02d.%02d.%04d %02d:%02d:%02d", date.dayOfMonth, date.month, date.year, date.hours, date.minutes, date.seconds), Colors::HHU_LIGHT_GRAY);

            lfb->placeString(font, 50, 24, "hhuOS main menu", Colors::HHU_BLUE);

            lfb->placeLine(33, 26, 66, 26, Colors::HHU_BLUE_50);

            lfb->placeRect(50, 50, 60, 60, Colors::HHU_LIGHT_GRAY);

            lfb->placeRect(50, 59, 60, 59, Colors::HHU_LIGHT_GRAY);

            for (uint32_t i = 0; i < sizeof(menuOptions) / sizeof(const char *); i++) {
                lfb->placeString(font, 50, static_cast<uint16_t>(36 + i * menuDistance), menuOptions[i],
                                 Colors::HHU_LIGHT_GRAY);
            }

#if (TEST_THREADING == 1)
            lfb->placeString(font, 50, 32, (char*) String::valueOf(threadSum, 10), Colors::WHITE);
#endif

            lfb->placeString(font, 50, 84, menuDescriptions[option], Colors::HHU_BLUE_30);

            lfb->placeString(font, 50, 92, "Please select an option using the arrow keys", Colors::HHU_LIGHT_GRAY);
            lfb->placeString(font, 50, 95, "and confirm your selection using the space key.", Colors::HHU_LIGHT_GRAY);

            lfb->placeRect(50, static_cast<uint16_t>(36 + option * menuDistance), 58, menuDistance,
                           Colors::HHU_BLUE_70);

            lfb->show();

#if (TEST_THREADING == 1)
            WorkerThread<uint32_t, uint32_t > *w1 = new WorkerThread<uint32_t, uint32_t >(worker, 20, callback);
            WorkerThread<uint32_t, uint32_t > *w2 = new WorkerThread<uint32_t, uint32_t >(worker, 40, callback);
            WorkerThread<uint32_t, uint32_t > *w3 = new WorkerThread<uint32_t, uint32_t >(worker, 80, callback);

            w1->start();
            w2->start();
            w3->start();
#endif
        } else {
            startSelectedApp();
        }
    }
}

void Application::startSelectedApp() {
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

    lfb->disableDoubleBuffering();
    lfb->clear();

    switch (option) {
        case 0:
            startShell();
            waitForCurrentApp();
            break;
        case 1: {
            startBugDefender();
            waitForCurrentApp();
            break;
        }
        case 2:
            startAntDemo();
            waitForCurrentApp();
            break;
        case 3:
            startMandelbrotDemo();
            waitForCurrentApp();
            break;
        case 4:
            startMouseApp();
            waitForCurrentApp();
            break;
        case 5:
            startLoopSoundDemo();
            waitForCurrentApp();
            break;
        case 6:
            startMemoryManagerDemo();
            waitForCurrentApp();
            break;
        case 7:
            startExceptionDemo();
            waitForCurrentApp();
            break;
        default:
            break;
    }

    delete currentApp;
}

void Application::startBugDefender() {

    currentApp = new SimpleThread([]{

        auto *timeService = Kernel::System::getService<Kernel::TimeService>();
        auto *lfb = Kernel::System::getService<Kernel::GraphicsService>()->getLinearFrameBuffer();

        lfb->init(640, 480, 16);
        lfb->enableDoubleBuffering();

        Game *game = new BugDefender();

        float currentTime = timeService->getSystemTime() / 1000.0f;
        float acc = 0.0f;
        float delta = 0.01667f; // 60Hz

        while (game->isRunning) {
            float newTime = timeService->getSystemTime() / 1000.0f;
            float frameTime = newTime - currentTime;
            if(frameTime > 0.25f)
                frameTime = 0.25f;
            currentTime = newTime;

            acc += frameTime;

            while(acc >= delta){
                game->update(delta);
                acc -= delta;
            }

            game->draw(lfb);
        }

        delete game;
    });

    currentApp->start();
}

void Application::waitForCurrentApp() {
    Kernel::System::getService<Kernel::EventBus>()->unsubscribe(*this, Kernel::KeyEvent::TYPE);

    currentApp->join();

    graphicsService->getLinearFrameBuffer()->init(xres, yres, bpp);

    // Don't use High-Res mode on CGA, as it looks bad.
    if(graphicsService->getLinearFrameBuffer()->getDepth() == 1) {
        graphicsService->getLinearFrameBuffer()->init(320, 200, 2);
    }

    graphicsService->getLinearFrameBuffer()->enableDoubleBuffering();

    Kernel::System::getService<Kernel::EventBus>()->subscribe(*this, Kernel::KeyEvent::TYPE);

    isRunning = true;
}

void Application::run() {

    timeService = Kernel::System::getService<Kernel::TimeService>();
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();
    Util::Array<String> res = Kernel::Multiboot::Structure::getKernelOption("resolution").split("x");

    if(res.length() >= 3) {
        xres = static_cast<uint16_t>(strtoint((const char *) res[0]));
        yres = static_cast<uint16_t>(strtoint((const char *) res[1]));
        bpp = static_cast<uint8_t>(strtoint((const char *) res[2]));

        lfb->init(xres, yres, bpp);

        // Don't use High-Res mode on CGA, as it looks bad.
        if(lfb->getDepth() == 1) {
            lfb->init(320, 200, 2);
        }
    } else {
        xres = lfb->getResX();
        yres = lfb->getResY();
        bpp = lfb->getDepth();
    }

    lfb->enableDoubleBuffering();

    Kernel::System::getService<Kernel::EventBus>()->subscribe(*this, Kernel::KeyEvent::TYPE);

    showMenu();
}

void Application::onEvent(const Kernel::Event &event) {

    auto &keyEvent = (Kernel::KeyEvent&) event;

    if (!keyEvent.getKey().isPressed()) {
        return;
    }

    switch (keyEvent.getKey().scancode()) {
        case Kernel::KeyEvent::SPACE:
            isRunning = false;
            break;
        case Kernel::KeyEvent::DOWN:
            if (option >= sizeof(menuOptions) / sizeof(const char *) - 1) {
                option = 0;
            } else {
                option++;
            }
            break;
        case Kernel::KeyEvent::UP:
            if (option <= 0) {
                option = sizeof(menuOptions) / sizeof(const char *) - 1;
            } else {
                option --;
            }
            break;
        default:
            break;
    }
}
