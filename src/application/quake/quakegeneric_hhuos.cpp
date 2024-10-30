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
#include "application/quake/quakegeneric/source/quakekeys.h"
#include "lib/util/async/Thread.h"
#include "lib/util/base/Exception.h"
#include "lib/util/base/String.h"
#include "lib/util/base/System.h"
#include "lib/util/collection/Array.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/io/stream/PrintStream.h"

uint32_t palette[256];
Util::Graphic::LinearFrameBuffer *lfb;
Util::Graphic::BufferedLinearFrameBuffer *bufferedlfb;
Util::Io::KeyDecoder *kd;

void (*drawFrame)(void *pixels);

int32_t main(int argc, char *argv[]) {
    if (!Util::Io::File::changeDirectory("/user/quake")) {
        Util::System::error << "quake: '/user/quake' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    // Prepare graphics
    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    auto lfbFile = new Util::Io::File("/device/lfb");

    // If '-res' is given, try to change display resolution
    for (int32_t i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-res") && i < argc - 1) {
            auto split1 = Util::String(argv[i + 1]).split("x");
            auto split2 = split1[1].split("@");

            uint32_t resolutionX = Util::String::parseInt(split1[0]);
            uint32_t resolutionY = Util::String::parseInt(split2[0]);
            uint32_t colorDepth = split2.length() > 1 ? Util::String::parseInt(split2[1]) : 32;

            lfbFile->controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
            break;
        }
    }

    // Use double buffering to improve tearing and automatically scale the image to the screen
    lfb = new Util::Graphic::LinearFrameBuffer(*lfbFile);
    bufferedlfb = new Util::Graphic::BufferedLinearFrameBuffer(*lfb, static_cast<uint16_t>(QUAKEGENERIC_RES_X), static_cast<uint16_t>(QUAKEGENERIC_RES_Y));

    // Initialize game
    QG_Create(argc, argv);

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

void QG_SetPalette(uint8_t source[768]) {
    for (uint32_t i = 0; i < 256; i++) {
        auto color = Util::Graphic::Color(source[i * 3], source[i * 3 + 1], source[i * 3 + 2]);
        palette[i] = color.getColorForDepth(lfb->getColorDepth());
    }
}

void QG_DrawFrame(void *pixels) {
    if (bufferedlfb == nullptr) {
        return;
    }

    drawFrame(pixels);
    bufferedlfb->flush();
}

void QG_DrawFrame32(void *pixels) {
    auto pixelBuffer = reinterpret_cast<const uint8_t*>(pixels);
    auto screenBuffer = reinterpret_cast<uint32_t*>(bufferedlfb->getBuffer().get());

    for (uint32_t y = 0; y < QUAKEGENERIC_RES_Y; y++) {
        for (uint32_t x = 0; x < QUAKEGENERIC_RES_X; x++) {
            auto pixel = pixelBuffer[y * QUAKEGENERIC_RES_X + x];
            auto color = palette[pixel];

            screenBuffer[x] = color;
        }

        screenBuffer += (bufferedlfb->getPitch() / 4);
    }
}

void QG_DrawFrame24(void *pixels) {
    auto pixelBuffer = reinterpret_cast<const uint8_t*>(pixels);
    auto screenBuffer = reinterpret_cast<uint8_t*>(bufferedlfb->getBuffer().get());

    for (uint32_t y = 0; y < QUAKEGENERIC_RES_Y; y++) {
        for (uint32_t x = 0; x < QUAKEGENERIC_RES_X; x++) {
            auto pixel = pixelBuffer[y * QUAKEGENERIC_RES_X + x];
            auto color = palette[pixel];

            screenBuffer[x * 3] = color & 0xff;
            screenBuffer[x * 3 + 1] = (color >> 8) & 0xff;
            screenBuffer[x * 3 + 2] = (color >> 16) & 0xff;
        }

        screenBuffer += (bufferedlfb->getPitch());
    }
}

void QG_DrawFrame16(void *pixels) {
    auto pixelBuffer = reinterpret_cast<const uint8_t*>(pixels);
    auto screenBuffer = reinterpret_cast<uint16_t *>(bufferedlfb->getBuffer().get());

    for (uint32_t y = 0; y < QUAKEGENERIC_RES_Y; y++) {
        for (uint32_t x = 0; x < QUAKEGENERIC_RES_X; x++) {
            auto pixel = pixelBuffer[y * QUAKEGENERIC_RES_X + x];
            auto color = palette[pixel];

            screenBuffer[x] = color;
        }

        screenBuffer += (bufferedlfb->getPitch() / 2);
    }
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

void QG_Init(void) {
    kd = new Util::Io::KeyDecoder(new Util::Io::DeLayout());

    switch (lfb->getColorDepth()) {
        case 32:
            drawFrame = &QG_DrawFrame32;
            break;
        case 24:
            drawFrame = &QG_DrawFrame24;
            break;
        case 15:
        case 16:
            drawFrame = &QG_DrawFrame16;
            break;
        default:
            Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Unsupported color depth!");
    }
}

void QG_Quit(void) {
    Util::Graphic::Ansi::cleanupGraphicalApplication();
    delete lfb;
    delete kd;
}