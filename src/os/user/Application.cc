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
#include "kernel/threads/Scheduler.h"
#include "user/Application.h"
#include "user/LoopSoundApp/Loop.h"
#include "user/LoopSoundApp/Sound.h"
#include "user/HeapApp/HeapDemo.h"
#include "user/AntApp/AntApp.h"
#include "user/VFSApp/VFSApp.h"
#include "user/MemoryTestApps/IOMemoryTestApp.h"
#include "user/AsciimationApp/AsciimationApp.h"
#include "lib/libc/snprintf.h"


#define MENU_DISTANCE 4
#define MENU_OPTIONS 9

Application::Application () : Thread ("Menu") {
    graphicsService = (GraphicsService *) Kernel::getService(GraphicsService::SERVICE_NAME);
	timeService = (TimeService*) Kernel::getService(TimeService::SERVICE_NAME);
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
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();
    lfb->init(640, 480, 32);

    Thread *thread = new AsciimationApp(""); // TODO fix (Fabian)
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

void Application::vfsDemo() {
    TextDriver *stream = graphicsService->getTextDriver();
    stream->init(100, 37, 32);

    Thread *thread = new VFSApp();
    thread->start();
}

void Application::showMenu () {
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

        lfb->placeString(font, 50, 42 + 0 * MENU_DISTANCE, "Loops and Sound", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 1 * MENU_DISTANCE, "Heap", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 2 * MENU_DISTANCE, "Protected Mode", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 3 * MENU_DISTANCE, "Langstons Ant", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 4 * MENU_DISTANCE, "Asciimation", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 5 * MENU_DISTANCE, "VFS Demo", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 6 * MENU_DISTANCE, "IO Memory Manager Test", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 7 * MENU_DISTANCE, "Mouse", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 42 + 8 * MENU_DISTANCE, "Bug Defender", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 90, "Please select an option using the arrow keys", Colors::HHU_LIGHT_GRAY);
        lfb->placeString(font, 50, 93, "and confirm your selection using the space key.", Colors::HHU_LIGHT_GRAY);

        lfb->placeRect(50, 42 + option * MENU_DISTANCE, 58, MENU_DISTANCE, Colors::HHU_BLUE_70);
        
        lfb->show();
    }
}

void Application::startSelectedApp() {
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

    lfb->disableDoubleBuffering();
    lfb->clear();
    Pit::getInstance()->setCursor(true);

    switch (option) {
        case 0:
            LoopSound ();
            break;
        case 1:
            Heap ();
            break;
        case 2:
            ProtectedMode ();
            break;
        case 3:
            Ant ();
            break;
        case 4:
            Asciimation();
            break;
        case 5:
            vfsDemo();
            break;
        case 6:
            IOMemoryTest();
            break;
        case 7:
            startMouseApp();
            break;
        case 8:
            Game *game = new BugDefender();
            startGame(game);
            break;
    }
}

void Application::startGame(Game* game){
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

    lfb->init(640, 480, 32);
    lfb->enableDoubleBuffering();
    Pit::getInstance()->setCursor(false);
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
    timeService = (TimeService *) Kernel::getService(TimeService::SERVICE_NAME);
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

    Pit::getInstance()->setCursor(false);
    lfb->init(800, 600, 32);
    lfb->enableDoubleBuffering();

    ((EventBus*) Kernel::getService(EventBus::SERVICE_NAME))->subscribe(*this, KeyEvent::TYPE);

    showMenu();

    ((EventBus*) Kernel::getService(EventBus::SERVICE_NAME))->unsubscribe(*this, KeyEvent::TYPE);

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
            option++;
            if (option >= MENU_OPTIONS) option = 0;
            break;
            // Up
        case 72:
            option--;
            if (option < 0) option = MENU_OPTIONS-1;
            break;
    }
}
