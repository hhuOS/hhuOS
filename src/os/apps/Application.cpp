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

#include <apps/BugDefender/BugDefender.h>
#include <devices/timer/Pit.h>
#include <lib/libc/printf.h>
#include <apps/MouseApp/MouseApp.h>
#include <kernel/services/DebugService.h>
#include <apps/Shell/Shell.h>
#include <lib/multiboot/Structure.h>
#include <kernel/threads/Scheduler.h>
#include <apps/MemoryTests/HeapTestApp.h>
#include <apps/MemoryTests/IOMemoryTestApp.h>
#include <kernel/threads/WorkerThread.h>
#include <apps/LoopsAndSound/LoopsAndSound.h>
#include <apps/Mandelbrot/Mandelbrot.h>
#include <apps/MemoryTests/MemoryManagerTest.h>
#include <kernel/memory/manager/FreeListMemoryManager.h>
#include <kernel/memory/manager/BitmapMemoryManager.h>
#include <kernel/memory/manager/BitmapMemoryManager.h>
#include "apps/LoopsAndSound/Loop.h"
#include "apps/LoopsAndSound/Sound.h"
#include "apps/AntApp/AntApp.h"
#include "apps/AsciimationApp/AsciimationApp.h"
#include "lib/libc/snprintf.h"
#include "Application.h"


#define TEST_THREADING 0

Thread *currentApp = nullptr;

uint32_t threadSum = 0;

uint32_t worker(const uint32_t &number) {

    return number * 2;
}

void callback(const Thread &thread, const uint32_t &number) {

    threadSum += number;
}

Application *Application::instance = nullptr;

Application::Application () : Thread ("Menu") {
    graphicsService = Kernel::getService<GraphicsService>();
	timeService = Kernel::getService<TimeService>();
}

Application *Application::getInstance() {
    if(instance == nullptr) {
        instance = new Application();
    }

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

void Application::startIoMemoryDemo() {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

    currentApp = new IOMemoryTestApp();
    currentApp->start();
}

void Application::startAsciimationDemo() {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

    currentApp = new AsciimationApp();

    currentApp->start();
}

void Application::startHeapDemo() {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

    currentApp = new HeapTestApp();

    currentApp->start();
}

void Application::startMemoryManagerDemo() {
    Keyboard *kb = Kernel::getService<InputService>()->getKeyboard();
    TextDriver *stream = graphicsService->getTextDriver();

    stream->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

    *stream << "===MemoryManagerTest===" << endl;
    *stream << "===Testing FreeListMemoryManager===" << endl << endl;
    MemoryManagerTest<FreeListMemoryManager> freeListTest(1048576, 128);
    freeListTest.run();

    printf("\nPress [ENTER] to return");
    while (!kb->isKeyPressed(KeyEvent::RETURN));
    while (kb->isKeyPressed(KeyEvent::RETURN));

    stream->clear();

    *stream << "===MemoryManagerTest===" << endl;
    *stream << "===Testing BuddyMemoryManager===" << endl << endl;
    MemoryManagerTest<BuddyMemoryManager> buddyTest(1048576, 128);
    buddyTest.run();

    printf("\nPress [ENTER] to return");
    while (!kb->isKeyPressed(KeyEvent::RETURN));
    while (kb->isKeyPressed(KeyEvent::RETURN));

    stream->clear();

    *stream << "===MemoryManagerTest===" << endl;
    *stream << "===Testing BitmapMemoryManager===" << endl << endl;
    MemoryManagerTest<BitmapMemoryManager> bitmapTest(1048576, 128, 128);
    bitmapTest.run();

    printf("\nPress [ENTER] to return");
    while (!kb->isKeyPressed(KeyEvent::RETURN));
    while (kb->isKeyPressed(KeyEvent::RETURN));

    graphicsService->getLinearFrameBuffer()->init(xres, yres, bpp);

    // Don't use High-Res mode on CGA, as it looks bad.
    if(graphicsService->getLinearFrameBuffer()->getDepth() == 1) {
        graphicsService->getLinearFrameBuffer()->init(320, 200, 2);
    }

    graphicsService->getLinearFrameBuffer()->enableDoubleBuffering();
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
    Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Y U DO DIS? :(");
}

void Application::startShell() {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

    currentApp = new Shell();

    currentApp->start();
}

void Application::showMenu () {
    while(true) {

        LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

        Font &font = lfb->getResY() < 400 ? (Font&) std_font_8x8 : (Font&) sun_font_8x16;

        if(isRunning) {
            Rtc::Date date = timeService->getRTC()->getCurrentDate();
            char timeString[20];
            snprintf(timeString, 20, "%02d.%02d.%04d %02d:%02d:%02d", date.dayOfMonth, date.month, date.year,
                     date.hours, date.minutes, date.seconds);

            lfb->placeRect(50, 50, 98, 98, Colors::HHU_LIGHT_GRAY);

            lfb->placeString(font, 50, 12, timeString, Colors::HHU_LIGHT_GRAY);

            lfb->placeString(font, 50, 24, "hhuOS main menu", Colors::HHU_BLUE);

            lfb->placeLine(33, 26, 66, 26, Colors::HHU_BLUE_50);

            lfb->placeRect(50, 50, 60, 60, Colors::HHU_LIGHT_GRAY);

            lfb->placeRect(50, 59, 60, 59, Colors::HHU_LIGHT_GRAY);

            for (uint32_t i = 0; i < sizeof(menuOptions) / sizeof(const char *); i++) {
                lfb->placeString(font, 50, static_cast<uint16_t>(35 + i * menuDistance), menuOptions[i],
                                 Colors::HHU_LIGHT_GRAY);
            }

#if (TEST_THREADING == 1)
            lfb->placeString(font, 50, 32, (char*) String::valueOf(threadSum, 10), Colors::WHITE);
#endif

            lfb->placeString(font, 50, 84, menuDescriptions[option], Colors::HHU_BLUE_30);

            lfb->placeString(font, 50, 92, "Please select an option using the arrow keys", Colors::HHU_LIGHT_GRAY);
            lfb->placeString(font, 50, 95, "and confirm your selection using the space key.", Colors::HHU_LIGHT_GRAY);

            lfb->placeRect(50, static_cast<uint16_t>(35 + option * menuDistance), 58, menuDistance,
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
            pause();
            break;
        case 1: {
            Game *game = new BugDefender();
            startGame(game);
            delete game;
            break;
        }
        case 2:
            startAsciimationDemo();
            pause();
            break;
        case 3:
            startAntDemo();
            pause();
            break;
        case 4:
            startMandelbrotDemo();
            pause();
            break;
        case 5:
            startMouseApp();
            pause();
            break;
        case 6:
            startLoopSoundDemo();
            pause();
            break;
        case 7:
            startHeapDemo();
            pause();
            break;
        case 8:
            startIoMemoryDemo();
            pause();
            break;
        case 9:
            startMemoryManagerDemo();
            isRunning = true;
            break;
        case 10:
            startExceptionDemo();
            pause();
            break;
        default:
            break;
    }

    if(currentApp != nullptr) {
        delete currentApp;
        currentApp = nullptr;
    }
}

void Application::startGame(Game* game){
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

    lfb->init(640, 480, 16);
    lfb->enableDoubleBuffering();

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

    graphicsService->getLinearFrameBuffer()->init(xres, yres, bpp);

    // Don't use High-Res mode on CGA, as it looks bad.
    if(graphicsService->getLinearFrameBuffer()->getDepth() == 1) {
        graphicsService->getLinearFrameBuffer()->init(320, 200, 2);
    }

    graphicsService->getLinearFrameBuffer()->enableDoubleBuffering();

    isRunning = true;
}

void Application::pause() {
    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);
    Scheduler::getInstance()->block();
}

void Application::resume() {
    isRunning = true;

    graphicsService->getLinearFrameBuffer()->init(xres, yres, bpp);

    // Don't use High-Res mode on CGA, as it looks bad.
    if(graphicsService->getLinearFrameBuffer()->getDepth() == 1) {
        graphicsService->getLinearFrameBuffer()->init(320, 200, 2);
    }

    graphicsService->getLinearFrameBuffer()->enableDoubleBuffering();

    Scheduler::getInstance()->deblock(*this);
    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);
}

void Application::run() {

    timeService = Kernel::getService<TimeService>();
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();
    Util::Array<String> res = Multiboot::Structure::getKernelOption("vbe").split("x");

    xres = lfb->getResX();
    yres = lfb->getResY();
    bpp = lfb->getDepth();
    
    lfb->enableDoubleBuffering();

    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);

    showMenu();
}

void Application::onEvent(const Event &event) {

    auto &keyEvent = (KeyEvent&) event;

    if (!keyEvent.getKey().isPressed()) {
        return;
    }

    switch (keyEvent.getKey().scancode()) {
        case KeyEvent::SPACE:
            isRunning = false;
            break;
        case KeyEvent::DOWN:
            if (option >= sizeof(menuOptions) / sizeof(const char *) - 1) {
                option = 0;
            } else {
                option++;
            }
            break;
        case KeyEvent::UP:
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
