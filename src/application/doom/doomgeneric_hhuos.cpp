/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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
 *
 * The doom port is based on a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#include <time.h>
#include <ctype.h>

#include "lib/util/async/SharedMemory.h"
#include "doomgeneric/doomgeneric/i_video.h"
#include "doomgeneric/doomgeneric/doomgeneric.h"
#include "doomgeneric/doomgeneric/doomkeys.h"
#include "kepler/Window.h"
#include "lib/interface.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/Address.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/graphic/font/Terminal8x8.h"
#include "lib/util/graphic/Colors.h"
#include "util/collection/ArrayQueue.h"

uint32_t palette[256];
Util::Graphic::LinearFrameBuffer *lfb;
const Kepler::Window *win;
Util::String initialTitle = "Doom";
Util::Io::KeyDecoder *kd;
Util::ArrayQueue<Util::Io::KeyEvent> keyEvents(32);

uint8_t scaleFactor = 0;
uint16_t offsetX = 0;
uint16_t offsetY = 0;
uint16_t drawResX = 0;
uint16_t drawResY = 0;

Util::Time::Timestamp lastFrameTime;
Util::Time::Timestamp fpsTimer;
uint32_t fpsCounter = 0;
uint32_t fps = 0;

bool triangleRunning = true;

void (*drawFrame)();

class EventListener : public Kepler::EventListener {

public:

    EventListener() = default;

    ~EventListener() override = default;

    void onKeyEvent(const Util::Io::KeyEvent &key) override {
        keyEvents.offer(key);
    }

    void onCloseButtonPressed() override {
        triangleRunning = false;
    }
};

void DG_DrawFrame32() {
    auto screenBuffer = reinterpret_cast<uint32_t*>(lfb->getBuffer().add(offsetX * 4 + offsetY * lfb->getPitch()).get());

    for (uint16_t y = 0; y < drawResY; y++) {
        for (uint16_t x = 0; x < drawResX; x++) {
            auto pixel = DG_ScreenBuffer[(y / scaleFactor) * DOOMGENERIC_RESX + (x / scaleFactor)];
            auto color = palette[pixel];

            screenBuffer[x] = color;
        }

        screenBuffer += (lfb->getPitch() / 4);
    }
}

void DG_DrawFrame24() {
    auto screenBuffer = reinterpret_cast<uint8_t*>(lfb->getBuffer().add(offsetX * 3 + offsetY * lfb->getPitch()).get());

    for (uint16_t y = 0; y < drawResY; y++) {
        for (uint16_t x = 0; x < drawResX; x++) {
            auto pixel = DG_ScreenBuffer[(y / scaleFactor) * DOOMGENERIC_RESX + (x / scaleFactor)];
            auto color = palette[pixel];

            screenBuffer[x * 3] = color & 0xff;
            screenBuffer[x * 3 + 1] = (color >> 8) & 0xff;
            screenBuffer[x * 3 + 2] = (color >> 16) & 0xff;
        }

        screenBuffer += (lfb->getPitch());
    }
}

void DG_DrawFrame16() {
    auto screenBuffer = reinterpret_cast<uint16_t*>(lfb->getBuffer().add(offsetX * 2 + offsetY * lfb->getPitch()).get());

    for (uint16_t y = 0; y < drawResY; y++) {
        for (uint16_t x = 0; x < drawResX; x++) {
            auto pixel = DG_ScreenBuffer[(y / scaleFactor) * DOOMGENERIC_RESX + (x / scaleFactor)];
            auto color = palette[pixel];

            screenBuffer[x] = color;
        }

        screenBuffer += (lfb->getPitch() / 2);
    }
}

int32_t main(int argc, char **argv) {
    if (!Util::Io::File::changeDirectory("/user/doom")) {
        Util::System::error << "doomgeneric: '/user/doom' not found!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

	doomgeneric_Create(argc, argv);

    const auto window = Kepler::Window(640, 400, initialTitle);
    lfb = &window.getFrameBuffer();
    win = &window;

    EventListener eventListener;
    window.registerEventListener(eventListener);

    // Calculate scale factor the game as large as possible
    scaleFactor = lfb->getResolutionX() / DOOMGENERIC_RESX;
    if (lfb->getResolutionY() / DOOMGENERIC_RESY < scaleFactor) {
        scaleFactor = lfb->getResolutionY() / DOOMGENERIC_RESY;
    }
    if (scaleFactor == 0) {
        scaleFactor = 1;
    }

    // Calculate offset to center the game
    offsetX = lfb->getResolutionX() - DOOMGENERIC_RESX * scaleFactor > 0 ? (lfb->getResolutionX() - DOOMGENERIC_RESX * scaleFactor) / 2 : 0;
    offsetY = lfb->getResolutionY() - DOOMGENERIC_RESY * scaleFactor > 0 ? (lfb->getResolutionY() - DOOMGENERIC_RESY * scaleFactor) / 2 : 0;

    // Calculate resolution to draw
    drawResX = DOOMGENERIC_RESX * scaleFactor > lfb->getResolutionX() ? lfb->getResolutionX() : DOOMGENERIC_RESX * scaleFactor;
    drawResY = DOOMGENERIC_RESY * scaleFactor > lfb->getResolutionY() ? lfb->getResolutionY() : DOOMGENERIC_RESY * scaleFactor;

    // Generate color palette
    for (uint32_t i = 0; i < 256; i++) {
        auto color = Util::Graphic::Color(colors[i].r, colors[i].g, colors[i].b, colors[i].a);
        palette[i] = color.getColorForDepth(lfb->getColorDepth());
    }

    // Set draw method based on color depth
    switch (lfb->getColorDepth()) {
        case 32:
            drawFrame = &DG_DrawFrame32;
            break;
        case 24:
            drawFrame = &DG_DrawFrame24;
            break;
        case 15:
        case 16:
            drawFrame = &DG_DrawFrame16;
            break;
        default:
            Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION, "Unsupported color depth!");
    }

    lfb->clear();

    // Run game loop
    auto oldTime = Util::Time::Timestamp::getSystemTime();
    while (triangleRunning) {
        auto newTime = Util::Time::Timestamp::getSystemTime();
        if (oldTime == newTime) {
            Util::Async::Thread::yield();
        } else {
            doomgeneric_Tick();
        }
    }

    window.close();
    return 0;
}

void DG_Init() {
	kd = new Util::Io::KeyDecoder(Util::Io::DeLayout());
}

void DG_DrawFrame() {
    if (lfb == nullptr) {
        return;
    }

    drawFrame();

    lfb->drawString(Util::Graphic::Fonts::TERMINAL_8x8, 0, 0, static_cast<const char*>(Util::String::format("FPS: %02u", fps)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::BLACK);

    fpsCounter++;
    fpsTimer += (Util::Time::Timestamp::getSystemTime() - lastFrameTime);
    lastFrameTime = Util::Time::Timestamp::getSystemTime();

    if (fpsTimer >= Util::Time::Timestamp::ofSeconds(1)) {
        fps = fpsCounter;
        fpsCounter = 0;
        fpsTimer = Util::Time::Timestamp();
    }

    win->flush();
}

void DG_SleepMs(uint32_t ms) {
	Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(ms));
}

uint32_t DG_GetTicksMs() {
	return clock();
}

int DG_GetKey(int *pressed, unsigned char *key) {
	if (keyEvents.isEmpty()) {
		return 0;
	}

    const auto &keyEvent = keyEvents.poll();

    *pressed = keyEvent.isPressed() ? 1 : 0;

    switch(keyEvent.getScancode()) {
        case 0: {
            if (keyEvent.getCtrlLeft()) {
                *key = KEY_FIRE;
                return 1;
            }
            return 0;
        }
        case Util::Io::KeyEvent::UP:
            *key = KEY_UPARROW;
            return 1;
        case Util::Io::KeyEvent::DOWN:
            *key = KEY_DOWNARROW;
            return 1;
        case Util::Io::KeyEvent::LEFT:
            *key = KEY_LEFTARROW;
            return 1;
        case Util::Io::KeyEvent::RIGHT:
            *key = KEY_RIGHTARROW;
            return 1;
        case Util::Io::KeyEvent::SPACE:
            *key = KEY_USE;
            return 1;
        case Util::Io::KeyEvent::ESC:
            *key = KEY_ESCAPE;
            return 1;
        case Util::Io::KeyEvent::ENTER:
            *key = KEY_ENTER;
            return 1;
        case Util::Io::KeyEvent::TAB:
            *key = KEY_TAB;
            return 1;
        case Util::Io::KeyEvent::BACKSPACE:
            *key = KEY_BACKSPACE;
            return 1;
        case Util::Io::KeyEvent::HOME:
            *key = KEY_HOME;
            return 1;
        case Util::Io::KeyEvent::END:
            *key = KEY_END;
            return 1;
        case Util::Io::KeyEvent::INSERT:
            *key = KEY_INS;
            return 1;
        case Util::Io::KeyEvent::DEL:
            *key = KEY_DEL;
            return 1;
        case Util::Io::KeyEvent::PAGE_UP:
            *key = KEY_PGUP;
            return 1;
        case Util::Io::KeyEvent::PAGE_DOWN:
            *key = KEY_PGDN;
            return 1;
        case Util::Io::KeyEvent::F1:
            *key = KEY_F1;
            return 1;
        case Util::Io::KeyEvent::F2:
            *key = KEY_F2;
            return 1;
        case Util::Io::KeyEvent::F3:
            *key = KEY_F3;
            return 1;
        case Util::Io::KeyEvent::F4:
            *key = KEY_F4;
            return 1;
        case Util::Io::KeyEvent::F5:
            *key = KEY_F5;
            return 1;
        case Util::Io::KeyEvent::F6:
            *key = KEY_F6;
            return 1;
        case Util::Io::KeyEvent::F7:
            *key = KEY_F7;
            return 1;
        case Util::Io::KeyEvent::F8:
            *key = KEY_F8;
            return 1;
        case Util::Io::KeyEvent::F9:
            *key = KEY_F9;
            return 1;
        case Util::Io::KeyEvent::F10:
            *key = KEY_F10;
            return 1;
        case Util::Io::KeyEvent::F11:
            *key = KEY_F11;
            return 1;
        case Util::Io::KeyEvent::F12:
            *key = KEY_F12;
            return 1;
        default:
            if (keyEvent.getAscii()) {
                *key = tolower(keyEvent.getAscii());
                return 1;
            }

            return 0;
    }
}

void DG_SetWindowTitle([[maybe_unused]] const char *title) {
    if (win == nullptr) {
        initialTitle = title;
    } else {
        win->setTitle(title);
    }
}