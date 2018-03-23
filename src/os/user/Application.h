/*****************************************************************************
 *                                                                           *
 *                         A P P L I C A T I O N                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Einstieg in eine Anwendung.                              *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 24.8.2016                       *
 *****************************************************************************/
#ifndef __application_include__
#define __application_include__

#include <kernel/events/Receiver.h>
#include "kernel/threads/Thread.h"
#include "user/game/Game.h"
#include "devices/Rtc.h"
#include "kernel/services/TimeService.h"
#include "kernel/services/GraphicsService.h"

class Application : public Thread, public Receiver {

private:

    uint16_t xres = 800;
    uint16_t yres = 600;
    uint8_t bpp = 32;

    Application (const Application &copy); // Verhindere Kopieren

    TimeService *timeService;

    // Demos
    void LoopSound ();
    void Heap ();
    void ProtectedMode ();
    void Ant ();
    void shell();
    void Asciimation ();
    void IOMemoryTest();
    void showMenu();
    void startSelectedApp();
    void startMouseApp();

    GraphicsService *graphicsService;
    uint8_t option = 0;
    bool isRunning = true;

public:
    // Gib dem Anwendungsthread einen Stack.
    explicit Application ();

    ~Application() {};

    // Thread-Startmethode
    void run ();

    void onEvent(const Event &event) override;

    void startGame(Game *game);

};

#endif
