/*****************************************************************************
 *                                                                           *
 *                         A P P L I C A T I O N                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Einstieg in eine Anwendung.                              *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 21.8.2016                       *
 *****************************************************************************/

#include <apps/BugDefender/BugDefender.h>
#include <devices/Pit.h>
#include <lib/libc/printf.h>
#include <apps/MouseApp/MouseApp.h>
#include <kernel/services/DebugService.h>
#include <apps/Shell/Shell.h>
#include <lib/multiboot/Structure.h>
#include <kernel/threads/Scheduler.h>
#include <apps/MemoryAreaTests/HeapTestApp.h>
#include <apps/MemoryAreaTests/IOMemoryTestApp.h>
#include <kernel/threads/WorkerThread.h>
#include <apps/LoopsAndSound/LoopsAndSound.h>
#include "apps/LoopsAndSound/Loop.h"
#include "apps/LoopsAndSound/Sound.h"
#include "apps/AntApp/AntApp.h"
#include "apps/AsciimationApp/AsciimationApp.h"
#include "lib/libc/snprintf.h"

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
    currentApp = new LoopsAndSound();

    currentApp->start();
}



void Application::startAntDemo() {
    currentApp = new AntApp();

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



/*****************************************************************************
 * Methode:         Application::Heap                                        *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Heap demo.                                               *
 *****************************************************************************/
void Application::startHeapDemo() {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

    currentApp = new HeapTestApp();

    currentApp->start();
}

void Application::startMouseApp() {
    graphicsService->getLinearFrameBuffer()->enableDoubleBuffering();

    currentApp = new MouseApp();

    currentApp->start();
}

void Application::startExceptionDemo() {
    uint32_t a = 1 - 1;
    uint32_t b = 2;
    printf("%d", b / a);
}

void Application::startShell() {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(static_cast<uint16_t>(xres / 8), static_cast<uint16_t>(yres / 16), bpp);

    currentApp = new Shell();

    currentApp->start();
}

void Application::showMenu () {
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

    Font &font = lfb->getResY() < 400 ? (Font&) std_font_8x8 : (Font&) sun_font_8x16;
    
    while(true) {
        if(isRunning) {
            Rtc::Date date = timeService->getRTC()->getCurrentDate();
            char timeString[20];
            snprintf(timeString, 20, "%02d.%02d.%04d %02d:%02d:%02d", date.dayOfMonth, date.month, date.year,
                     date.hours, date.minutes, date.seconds);

            lfb->placeRect(50, 50, 98, 98, Colors::HHU_LIGHT_GRAY);

            lfb->placeString(font, 50, 12, timeString, Colors::HHU_LIGHT_GRAY);

            lfb->placeString(font, 50, 25, "hhuOS main menu", Colors::HHU_BLUE);

            lfb->placeLine(33, 27, 66, 27, Colors::HHU_BLUE_50);

            lfb->placeRect(50, 50, 60, 60, Colors::HHU_LIGHT_GRAY);

            lfb->placeRect(50, 55, 60, 50, Colors::HHU_LIGHT_GRAY);

            for (uint32_t i = 0; i < sizeof(menuOptions) / sizeof(const char *); i++) {
                lfb->placeString(font, 50, static_cast<uint16_t>(42 + i * menuDistance), menuOptions[i],
                                 Colors::HHU_LIGHT_GRAY);
            }

#if (TEST_THREADING == 1)
            lfb->placeString(font, 50, 35, (char*) String::valueOf(threadSum, 10), Colors::WHITE);
#endif

            lfb->placeString(font, 50, 85, menuDescriptions[option], Colors::HHU_BLUE_30);

            lfb->placeString(font, 50, 90, "Please select an option using the arrow keys", Colors::HHU_LIGHT_GRAY);
            lfb->placeString(font, 50, 93, "and confirm your selection using the space key.", Colors::HHU_LIGHT_GRAY);

            lfb->placeRect(50, static_cast<uint16_t>(42 + option * menuDistance), 58, menuDistance,
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
            startMouseApp();
            pause();
            break;
        case 5:
            startLoopSoundDemo();
            pause();
            break;
        case 6:
            startHeapDemo();
            pause();
            break;
        case 7:
            startIoMemoryDemo();
            pause();
            break;
        case 8:
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
    lfb->enableDoubleBuffering();

    float currentTime = timeService->getSystemTime() / 100.0f;
    float acc = 0.0f;
    float delta = 0.01667f; // 60Hz

    while (game->isRunning) {
        float newTime = timeService->getSystemTime() / 100.0f;
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

    isRunning = true;
}

void Application::pause() {
    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);
    Scheduler::getInstance()->block();
}

void Application::resume() {
    isRunning = true;
    graphicsService->getLinearFrameBuffer()->enableDoubleBuffering();
    Scheduler::getInstance()->deblock(*this);
    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);
}

void Application::run() {
    timeService = Kernel::getService<TimeService>();
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();
    Util::Array<String> res = Multiboot::Structure::getKernelOption("vbe").split("x");

    if(res.length() >= 3) {
        xres = static_cast<uint16_t>(strtoint((const char *) res[0]));
        yres = static_cast<uint16_t>(strtoint((const char *) res[1]));
        bpp = static_cast<uint8_t>(strtoint((const char *) res[2]));
    }
    
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
            // Up
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
