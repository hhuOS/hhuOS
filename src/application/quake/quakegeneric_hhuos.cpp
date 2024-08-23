/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#include <time.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "quakegeneric/source/quakegeneric.h"

#include "lib/util/base/Address.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/game/Engine.h"

uint8_t palette[768];
Util::Graphic::LinearFrameBuffer *lfb;
Util::Graphic::BufferedLinearFrameBuffer *bufferedlfb;
Util::Io::KeyDecoder *kd;

uint32_t scaleFactor = 1;
uint32_t offsetX = 0;
uint32_t offsetY = 0;

int32_t main(int argc, char *argv[]) {
    if (!Util::Io::File::changeDirectory("/user/quake")) {
        Util::System::error << "quake: '/user/quake' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    QG_Create(argc, argv);

    // Prepare graphics
    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    auto lfbFile = new Util::Io::File("/device/lfb");

    // If '-res is given, try to change display resolution
    for (int32_t i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-res") && i < argc - 1) {
            auto resSplit = Util::String(argv[i + 1]).split("x");
            uint32_t x = Util::String::parseInt(resSplit[0]);
            uint32_t y = Util::String::parseInt(resSplit[1]);
            lfbFile->controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({x, y, 32}));

            break;
        }
    }

    // Use double buffering to avoid tearing
    lfb = new Util::Graphic::LinearFrameBuffer(*lfbFile);
    bufferedlfb = new Util::Graphic::BufferedLinearFrameBuffer(*lfb);

    // Calculate scale factor the game as large as possible
    scaleFactor = lfb->getResolutionX() / QUAKEGENERIC_RES_X;
    if (lfb->getResolutionY() / QUAKEGENERIC_RES_Y < scaleFactor) {
        scaleFactor = lfb->getResolutionY() / QUAKEGENERIC_RES_Y;
    }
    if (scaleFactor == 0) {
        scaleFactor = 1;
    }

    // Calculate offset to center the game
    offsetX = lfb->getResolutionX() - QUAKEGENERIC_RES_X * scaleFactor > 0 ? (lfb->getResolutionX() - QUAKEGENERIC_RES_X * scaleFactor) / 2 : 0;
    offsetY = lfb->getResolutionY() - QUAKEGENERIC_RES_Y * scaleFactor > 0 ? (lfb->getResolutionY() - QUAKEGENERIC_RES_Y * scaleFactor) / 2 : 0;

    // Run game loop
    auto oldTime = clock();
    while (true) {
        auto newTime = clock();
        if (oldTime == newTime) {
            Util::Async::Thread::yield();
        } else {
            QG_Tick((newTime - oldTime) / static_cast<double>(CLOCKS_PER_SEC));
            oldTime = newTime;
        }
    }
}

void QG_Init(void) {
    kd = new Util::Io::KeyDecoder(new Util::Io::DeLayout());
}

void QG_SetPalette(uint8_t source[768]) {
    memcpy(palette, source, 768);
}

void QG_DrawFrame(void *pixels) {
    if (bufferedlfb == nullptr) {
        return;
    }

    auto screenBuffer = reinterpret_cast<uint32_t*>(bufferedlfb->getBuffer().add(offsetX * 4 + offsetY * bufferedlfb->getPitch()).get());
    auto resX = QUAKEGENERIC_RES_X * scaleFactor > bufferedlfb->getResolutionX() ? bufferedlfb->getResolutionX() : QUAKEGENERIC_RES_X * scaleFactor;
    auto resY = QUAKEGENERIC_RES_Y * scaleFactor > bufferedlfb->getResolutionY() ? bufferedlfb->getResolutionY() : QUAKEGENERIC_RES_Y * scaleFactor;

    for (uint32_t y = 0; y < resY; y++) {
        for (uint32_t x = 0; x < resX; x++) {
            uint8_t pixel = reinterpret_cast<uint8_t*>(pixels)[(y / scaleFactor) * QUAKEGENERIC_RES_X + (x / scaleFactor)];
            uint8_t *paletteEntry = &((uint8_t*) palette)[pixel * 3];
            screenBuffer[x] = (*(paletteEntry) << 16) + (*(paletteEntry + 1) << 8) + *(paletteEntry + 2);
        }

        screenBuffer += (bufferedlfb->getPitch() / sizeof(uint32_t));
    }

    bufferedlfb->flush();
}

int QG_GetKey(int *down, int *key) {
    if (!stdin->isReadyToRead() || kd == nullptr) {
        return 0;
    } else {
        uint8_t scancode = fgetc(stdin);

        if ((scancode & ~0x80) == 0x1d) {
            *down = !(scancode & 0x80);
            *key = K_CTRL;
            return 1;
        }
        if ((scancode & ~0x80) == 0x38) {
            *down = !(scancode & 0x80);
            *key = K_ALT;
            return 1;
        }

        if (kd->parseScancode(scancode)) {
            auto k = kd->getCurrentKey();
            if (!k.isValid()) {
                return 0;
            }

            *down = k.isPressed() ? 1:0;

            if (k.getScancode() >= 0x3b && k.getScancode() <= 0x44) { // handle F1-10
                *key = k.getScancode() + 0x80;
                return 1;
            }

            switch(k.getScancode()) {
                case Util::Io::Key::UP:
                    *key = K_UPARROW;
                    return 1;
                case Util::Io::Key::DOWN:
                    *key = K_DOWNARROW;
                    return 1;
                case Util::Io::Key::LEFT:
                    *key = K_LEFTARROW;
                    return 1;
                case Util::Io::Key::RIGHT:
                    *key = K_RIGHTARROW;
                    return 1;
                case Util::Io::Key::SPACE:
                    *key = K_SPACE;
                    return 1;
                case Util::Io::Key::ESC:
                    *key = K_ESCAPE;
                    return 1;
                case Util::Io::Key::ENTER:
                    *key = K_ENTER;
                    return 1;
                case Util::Io::Key::TAB:
                    *key = K_TAB;
                    return 1;
                case Util::Io::Key::BACKSPACE:
                    *key = K_BACKSPACE;
                    return 1;
                case Util::Io::Key::HOME:
                    *key = K_HOME;
                    return 1;
                case Util::Io::Key::END:
                    *key = K_END;
                    return 1;
                case Util::Io::Key::INSERT:
                    *key = K_INS;
                    return 1;
                case Util::Io::Key::DEL:
                    *key = K_DEL;
                    return 1;
                case Util::Io::Key::PAGE_UP:
                    *key = K_PGUP;
                    return 1;
                case Util::Io::Key::PAGE_DOWN:
                    *key = K_PGDN;
                    return 1;
                case Util::Io::Key::F1:
                    *key = K_F1;
                    return 1;
                case Util::Io::Key::F2:
                    *key = K_F2;
                    return 1;
                case Util::Io::Key::F3:
                    *key = K_F3;
                    return 1;
                case Util::Io::Key::F4:
                    *key = K_F4;
                    return 1;
                case Util::Io::Key::F5:
                    *key = K_F5;
                    return 1;
                case Util::Io::Key::F6:
                    *key = K_F6;
                    return 1;
                case Util::Io::Key::F7:
                    *key = K_F7;
                    return 1;
                case Util::Io::Key::F8:
                    *key = K_F8;
                    return 1;
                case Util::Io::Key::F9:
                    *key = K_F9;
                    return 1;
                case Util::Io::Key::F10:
                    *key = K_F10;
                    return 1;
                case Util::Io::Key::F11:
                    *key = K_F11;
                    return 1;
                case Util::Io::Key::F12:
                    *key = K_F12;
                    return 1;
                default:
                    if (k.getAscii()) {
                        *key = tolower(k.getAscii());
                        return 1;
                    }

                    return 0;
            }
        }
    }

    return 0;
}

void QG_GetJoyAxes(float *axes) {
    *axes = 0;
}

void QG_GetMouseMove(int *x, int *y) {
    *x = 0;
    *y = 0;
}

void QG_Quit(void) {
    Util::Graphic::Ansi::cleanupGraphicalApplication();
    delete lfb;
    delete kd;
}