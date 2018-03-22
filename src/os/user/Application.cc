/*****************************************************************************
 *                                                                           *
 *                         A P P L I C A T I O N                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Einstieg in eine Anwendung.                              *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 21.8.2016                       *
 *****************************************************************************/

#include <user/BugDefender/BugDefender.h>
#include <devices/Pit.h>
#include <lib/libc/printf.h>
#include <user/MouseApp/MouseApp.h>
#include <kernel/services/DebugService.h>
#include <kernel/threads/WorkerThread.h>
#include <user/Shell/Shell.h>
#include <kernel/services/ModuleLoader.h>
#include "kernel/threads/Scheduler.h"
#include "user/Application.h"
#include "user/LoopSoundApp/Loop.h"
#include "user/LoopSoundApp/Sound.h"
#include "user/HeapApp/HeapDemo.h"
#include "user/AntApp/AntApp.h"
#include "user/MemoryTestApps/IOMemoryTestApp.h"
#include "user/AsciimationApp/AsciimationApp.h"
#include "lib/libc/snprintf.h"


#define MENU_DISTANCE 4
#define MENU_OPTIONS 9

#define TEST_THREADING 0

Application::Application () : Thread ("Menu") {
    graphicsService = Kernel::getService<GraphicsService>();
	timeService = Kernel::getService<TimeService>();
}

/*****************************************************************************
 * Methode:         Application::LoopSound                                   *
 *---------------------------------------------------------------------------*
 * Beschreibung:    2 counter threads und 1 sound thread.                    *
 *****************************************************************************/
void Application::LoopSound () {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(100, 37, 32);

    Thread *thread1 = new Loop(1);
    Thread *thread2 = new Loop(2);
    Thread *thread3 = new Sound(3);

    thread1->start();
    thread2->start();
    thread3->start();
}



void Application::Ant () {
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();
    lfb->init(800, 600, 32);

    Thread *thread = new AntApp();
    thread->start();
}

void Application::IOMemoryTest () {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(100, 37, 32);

    unsigned int* stack = new unsigned int[1024];
    IOMemoryTestApp* ioMemTest = new IOMemoryTestApp(&stack[1023]);

    Scheduler::getInstance()->ready(*ioMemTest);

    Scheduler::getInstance()->exit();
}

void Application::Asciimation () {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(100, 37, 32);

    Thread *thread = new AsciimationApp();
    thread->start();
}



/*****************************************************************************
 * Methode:         Application::Heap                                        *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Heap demo.                                               *
 *****************************************************************************/
void Application::Heap () {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(100, 37, 32);

    Thread *thread = new HeapDemo();
    thread->start();
}

void Application::startMouseApp() {
    MouseApp *mouseApp = new MouseApp();
    mouseApp->start();
}

/*
 Tests fuer ProtectedMode()
 */
void test_null_ptr_exc() {
    unsigned int *ptr = 0;
    *ptr = 1;
}


void badfunc() {
    unsigned int *ptr = (unsigned int*) (0xFF000000);
    *ptr = 1;
}

void test_access_outsideRAM() {
    badfunc();
}
/*****************************************************************************
 * Methode:         Application::ProtectedMode                               *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Protected-mode demo.                                     *
 *****************************************************************************/
void Application::ProtectedMode () {
    uint32_t a = 1 - 1;
    uint32_t b = 2;
    printf("%d", b / a);
}

void Application::shell() {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(100, 37, 32);

    Thread *thread = new Shell();
    thread->start();
}

uint32_t threadSum = 0;

uint32_t worker(const uint32_t &number) {

    return number * 2;
}

void callback(const Thread &thread, const uint32_t &number) {

    threadSum += number;
}

void Application::showMenu () {
    const char *descriptions[9] {
            "A simple UNIX-like Shell",
            "A fun game: Save the OS from invading bugs!",
            "Play an Asciimation file",
            "Watch Langtons Ant run around your screen",
            "A simple Demo, that uses the mouse",
            "Multi-Threading test",
            "Memory Management test for the Heap",
            "Memory Management test for IO Memory",
            "Bluescreen test",
    };

    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

    Font &font = lfb->getResY() < 400 ? (Font&) std_font_8x8 : (Font&) sun_font_8x16;
    
    while (isRunning) {

        Rtc::date date = timeService->getRTC()->getCurrentDate();
        char timeString[20];
        snprintf(timeString, 20, "%02d.%02d.%04d %02d:%02d:%02d", date.dayOfMonth, date.month, date.year, date.hours, date.minutes, date.seconds);

        lfb->placeRect(50, 50, 98, 98, Colors::HHU_LIGHT_GRAY);

        lfb->placeString(font, 50, 12, timeString, Colors::HHU_LIGHT_GRAY);

        lfb->placeString(font, 50, 25, "hhuOS main menu", Colors::HHU_BLUE);

        lfb->placeLine(33, 27, 66, 27, Colors::HHU_BLUE_50);

        lfb->placeRect(50, 50, 60, 60, Colors::HHU_LIGHT_GRAY);

        lfb->placeRect(50, 55, 60, 50, Colors::HHU_LIGHT_GRAY);

        lfb->placeString(font, 50, 42 + 0 * MENU_DISTANCE, "Shell", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 1 * MENU_DISTANCE, "Bug Defender", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 2 * MENU_DISTANCE, "Asciimation", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 3 * MENU_DISTANCE, "Langtons Ant", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 4 * MENU_DISTANCE, "Mouse", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 5 * MENU_DISTANCE, "Loops and Sound", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 6 * MENU_DISTANCE, "Heap Test", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 7 * MENU_DISTANCE, "IO Memory Manager Test", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 8 * MENU_DISTANCE, "Exceptions Test", Colors::HHU_LIGHT_GRAY);

#if (TEST_THREADING == 1)
        lfb->placeString(font, 50, 35, (char*) String::valueOf(threadSum, 10), Colors::WHITE);
#endif

        lfb->placeString(font, 50, 85, descriptions[option], Colors::HHU_BLUE_30);

        lfb->placeString(font, 50, 90, "Please select an option using the arrow keys", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 93, "and confirm your selection using the space key.", Colors::HHU_LIGHT_GRAY);

        lfb->placeRect(50, 42 + option * MENU_DISTANCE, 58, MENU_DISTANCE, Colors::HHU_BLUE_70);
        
        lfb->show();

#if (TEST_THREADING == 1)
        WorkerThread<uint32_t, uint32_t > *w1 = new WorkerThread<uint32_t, uint32_t >(worker, 20, callback);
        WorkerThread<uint32_t, uint32_t > *w2 = new WorkerThread<uint32_t, uint32_t >(worker, 40, callback);
        WorkerThread<uint32_t, uint32_t > *w3 = new WorkerThread<uint32_t, uint32_t >(worker, 80, callback);

        w1->start();
        w2->start();
        w3->start();
#endif
    }
}

void Application::startSelectedApp() {
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

    lfb->disableDoubleBuffering();
    lfb->clear();

    switch (option) {
        case 0:
            shell();
            break;
        case 1: {
            Game *game = new BugDefender();
            startGame(game);
            break;
        }
        case 2:
            Asciimation();
            break;
        case 3:
            Ant();
            break;
        case 4:
            startMouseApp();
            break;
        case 5:
            LoopSound();
            break;
        case 6:
            Heap();
            break;
        case 7:
            IOMemoryTest();
            break;
        case 8:
            ProtectedMode();
            break;
        default:
            break;
    }
}

void Application::startGame(Game* game){
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

    lfb->init(800, 600, 32);
    lfb->enableDoubleBuffering();
    lfb->clear();

    float currentTime = timeService->getSystemTime() / 100.0f;
    float acc = 0.0f;
    float delta = 0.01667f; // 60Hz

    while (game->isRunning){
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

}

/*****************************************************************************
 * Methode:         Application::run                                         *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Der Anwendungsthread erzeugt drei Threads die Zaehler    *
 *                  ausgeben und terminiert sich selbst.                     *
 *****************************************************************************/
void Application::run() {
    timeService = Kernel::getService<TimeService>();
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

    lfb->enableDoubleBuffering();

    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);

    showMenu();

    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);

    startSelectedApp();
}

void Application::onEvent(const Event &event) {

    KeyEvent &keyEvent = (KeyEvent&) event;

    if (!keyEvent.getKey().isPressed()) {
        return;
    }

    switch (keyEvent.getKey().scancode()) {
        // Space
        case 57:
            isRunning = false;
            break;
            // Down
        case 80:
            if (option >= MENU_OPTIONS - 1) {
                option = 0;
            } else {
                option++;
            }
            break;
            // Up
        case 72:
            if (option <= 0) {
                option = MENU_OPTIONS - 1;
            } else {
                option --;
            }
            break;
    }
}
