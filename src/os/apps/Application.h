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
#include "apps/game/Game.h"
#include "devices/Rtc.h"
#include "kernel/services/TimeService.h"
#include "kernel/services/GraphicsService.h"

class Application : public Thread, public Receiver {

private:
    const char *menuOptions[11] = {
            "Shell", "Bug Defender", "Asciimation",
            "Langtons Ant", "Mandelbrot", "Mouse", "Loops and Sound",
            "Kernel Heap Demo", "IO Memory Demo", "Memory Manager Demo", "Exception Demo"
    };

    const char *menuDescriptions[11] {
            "A simple UNIX-like Shell",
            "A fun game: Save the OS from invading bugs!",
            "Play an Asciimation file",
            "Watch Langtons Ant run around your screen",
            "Navigate through the Mandelbrot set",
            "A simple Demo, that uses the mouse",
            "Multi-Threading test",
            "Memory Management test for the Kernel Heap",
            "Memory Management test for IO Memory",
            "Memory Management test for all available memory managers",
            "Bluescreen test",
    };

    static const constexpr uint8_t menuDistance = 4;

    uint16_t xres = 800;
    uint16_t yres = 600;
    uint8_t bpp = 32;

    uint8_t option = 0;
    bool isRunning = true;

    static Application *instance;

    TimeService *timeService;
    GraphicsService *graphicsService;

    explicit Application();

    ~Application() override = default;

    void startShell();
    void startLoopSoundDemo();
    void startHeapDemo();
    void startExceptionDemo();
    void startAntDemo();
    void startMandelbrotDemo();
    void startAsciimationDemo();
    void startIoMemoryDemo();
    void startMemoryManagerDemo();

    void showMenu();
    void startSelectedApp();
    void startMouseApp();

public:
    Application (const Application &copy) = delete;


    static Application *getInstance();

    void pause();

    void resume();


    void run() override;

    void onEvent(const Event &event) override;

    void startGame(Game *game);

};

#endif
