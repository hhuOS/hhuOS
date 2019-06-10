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

#ifndef __application_include__
#define __application_include__

#include "kernel/event/Receiver.h"
#include "kernel/thread/Thread.h"
#include "lib/game/Game.h"
#include "device/time/Rtc.h"
#include "kernel/service/TimeService.h"
#include "kernel/service/GraphicsService.h"

class Application : public Thread, public Receiver {

private:
    const char *menuOptions[8] = {
            "Shell", "Bug Defender", "Langtons Ant", "Mandelbrot", "Mouse", "Loops and Sound", "Memory Manager Demo",
            "Exception Demo"
    };

    const char *menuDescriptions[8] {
            "A simple UNIX-like Shell",
            "A fun game: Save the OS from invading bugs!",
            "Watch Langtons Ant run around your screen",
            "Navigate through the Mandelbrot set",
            "A simple Demo, that uses the mouse",
            "Multi-Threading test",
            "Memory Management test for all available memory managers",
            "Bluescreen test",
    };

    static const constexpr uint8_t menuDistance = 5;

    uint16_t xres = 800;
    uint16_t yres = 600;
    uint8_t bpp = 32;

    uint8_t option = 0;
    bool isRunning = true;

    TimeService *timeService;
    GraphicsService *graphicsService;

    explicit Application();

    ~Application() override = default;

    void startShell();
    void startLoopSoundDemo();
    void startExceptionDemo();
    void startAntDemo();
    void startMandelbrotDemo();
    void startMemoryManagerDemo();

    void showMenu();
    void startSelectedApp();
    void startMouseApp();

public:

    Application (const Application &copy) = delete;

    static Application & getInstance() noexcept;

    void run() override;

    void onEvent(const Event &event) override;

    void waitForCurrentApp();

    void startBugDefender();

};

#endif
