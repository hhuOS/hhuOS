/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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
 */

#include <stdint.h>
#include <stdlib.h>

#include "fce.h"
#include "hal.h"
#include "util/async/BasicRunnable.h"
#include "util/async/Thread.h"
#include "util/base/ArgumentParser.h"
#include "util/base/String.h"
#include "util/base/System.h"
#include "util/graphic/Ansi.h"
#include "util/graphic/BufferedLinearFrameBuffer.h"
#include "util/graphic/Colors.h"
#include "util/graphic/LinearFrameBuffer.h"
#include "util/graphic/font/Terminal8x8.h"
#include "util/io/key/KeyDecoder.h"
#include "util/io/key/layout/DeLayout.h"
#include "util/io/stream/FileInputStream.h"
#include "util/time/Timestamp.h"

struct KeyState {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool a = false;
    bool b = false;
    bool select = false;
    bool start = false;
};

uint16_t offsetX = 0;
uint16_t offsetY = 0;
uint16_t drawResX = SCREEN_WIDTH;
uint16_t drawResY = SCREEN_HEIGHT;
uint8_t scale = 1;
Util::Graphic::BufferedLinearFrameBuffer *bufferedLfb = nullptr;
uint32_t colorPalette[64];

KeyState keys;
auto keyDecoder = Util::Io::KeyDecoder(Util::Io::DeLayout());

constexpr uint32_t TARGET_FPS = 60;
Util::Time::Timestamp targetFrameTime = Util::Time::Timestamp::ofMicroseconds(1000000 / TARGET_FPS);

Util::Time::Timestamp fpsTimer;
Util::Time::Timestamp lastTime;
uint32_t fpsCounter = 0;
uint32_t fps = 0;

void (*drawBackground)(uint32_t);
void (*flushBuf)(const PixelBuf*);

void nes_hal_init() {
    const auto colorDepth = bufferedLfb->getColorDepth();

    for (uint32_t i = 0; i < 64; i++) {
        const auto color = palette[i];
        colorPalette[i] = Util::Graphic::Color(color.r, color.g, color.b).getColorForDepth(colorDepth);
    }

    lastTime = Util::Time::Timestamp::getSystemTime();
}

void nes_set_bg_color32(const uint32_t color) {
    auto *screenBuffer = reinterpret_cast<uint32_t*>(bufferedLfb->getBuffer().add(offsetX * 4 + offsetY * bufferedLfb->getPitch()).get());

    for (uint32_t y = 0; y < drawResY; y++) {
        for (uint32_t x = 0; x < drawResX; x++) {
            screenBuffer[x] = color;
        }

        screenBuffer += bufferedLfb->getPitch() / 4;
    }
}

void nes_set_bg_color24(const uint32_t color) {
    auto screenBuffer = reinterpret_cast<uint8_t*>(bufferedLfb->getBuffer().add(offsetX * 3 + offsetY * bufferedLfb->getPitch()).get());

    for (uint32_t y = 0; y < drawResY; y++) {
        for (uint32_t x = 0; x < drawResX; x++) {
            screenBuffer[x * 3] = color & 0xff;
            screenBuffer[x * 3 + 1] = (color >> 8) & 0xff;
            screenBuffer[x * 3 + 2] = (color >> 16) & 0xff;
        }

        screenBuffer += bufferedLfb->getPitch();
    }
}

void nes_set_bg_color16(const uint32_t color) {
    auto *screenBuffer = reinterpret_cast<uint16_t*>(bufferedLfb->getBuffer().add(offsetX * 2 + offsetY * bufferedLfb->getPitch()).get());

    for (uint32_t y = 0; y < drawResY; y++) {
        for (uint32_t x = 0; x < drawResX; x++) {
            screenBuffer[x] = color;
        }

        screenBuffer += bufferedLfb->getPitch() / 2;
    }
}

void nes_set_bg_color([[maybe_unused]] const int c) {
    const auto &color = colorPalette[c];
    drawBackground(color);
}

void nes_flush_buf(PixelBuf *buf) {
    flushBuf(buf);
}

void nes_flush_buf32(const PixelBuf *buf) {
    auto *screenBuffer = reinterpret_cast<uint32_t*>(bufferedLfb->getBuffer().add(offsetX * 4 + offsetY * bufferedLfb->getPitch()).get());
    const auto width = bufferedLfb->getPitch() / 4;

    for (int32_t i = 0; i < buf->size; i++) {
        const auto &pixel = buf->buf[i];

        if (pixel.x >= 0 && pixel.y >= 0) {
            const auto &color = colorPalette[pixel.c];

            for (uint8_t sy = 0; sy < scale; sy++) {
                for (uint8_t sx = 0; sx < scale; sx++) {
                    const auto px = pixel.x * scale + sx;
                    const auto py = pixel.y * scale + sy;

                    screenBuffer[py * width + px] = color;
                }
            }
        }
    }
}

void nes_flush_buf24(const PixelBuf *buf) {
    auto *screenBuffer = reinterpret_cast<uint8_t*>(bufferedLfb->getBuffer().add(offsetX * 3 + offsetY * bufferedLfb->getPitch()).get());
    const auto width = bufferedLfb->getPitch();

    for (int32_t i = 0; i < buf->size; i++) {
        const auto &pixel = buf->buf[i];

        if (pixel.x >= 0 && pixel.y >= 0) {
            const auto &color = colorPalette[pixel.c];

            for (uint8_t sy = 0; sy < scale; sy++) {
                for (uint8_t sx = 0; sx < scale; sx++) {
                    const auto px = pixel.x * scale + sx;
                    const auto py = pixel.y * scale + sy;
                    const auto offset = py * width + px * 3;

                    screenBuffer[offset]     = color & 0xff;
                    screenBuffer[offset + 1] = (color >> 8) & 0xff;
                    screenBuffer[offset + 2] = (color >> 16) & 0xff;
                }
            }
        }
    }
}

void nes_flush_buf16(const PixelBuf *buf) {
    auto *screenBuffer = reinterpret_cast<uint16_t*>(bufferedLfb->getBuffer().add(offsetX * 2 + offsetY * bufferedLfb->getPitch()).get());
    const auto width = bufferedLfb->getPitch() / 2;

    for (int32_t i = 0; i < buf->size; i++) {
        const auto &pixel = buf->buf[i];

        if (pixel.x >= 0 && pixel.y >= 0) {
            const auto &color = colorPalette[pixel.c];

            for (uint8_t sy = 0; sy < scale; sy++) {
                for (uint8_t sx = 0; sx < scale; sx++) {
                    const auto px = pixel.x * scale + sx;
                    const auto py = pixel.y * scale + sy;

                    screenBuffer[py * width + px] = color;
                }
            }
        }
    }
}

void nes_flip_display() {
    const auto fpsString = Util::String::format("FPS: %u", fps);
    bufferedLfb->drawString(Util::Graphic::Fonts::TERMINAL_8x8, 0, 0, static_cast<const char*>(fpsString), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::BLACK);

    bufferedLfb->flush();
    bufferedLfb->clear();
}

int nes_key_state(const int b) {
    switch (b) {
        case 0: // On / Off
            return 1;
        case 1: // A
            return keys.a ? 1 : 0;
        case 2: // B
            return keys.b ? 1 : 0;
        case 3: // SELECT
            return keys.select ? 1 : 0;
        case 4: // START
            return keys.start ? 1 : 0;
        case 5: // UP
            return keys.up ? 1 : 0;
        case 6: // DOWN
            return keys.down ? 1 : 0;
        case 7: // LEFT
            return keys.left ? 1 : 0;
        case 8: // RIGHT
            return keys.right ? 1 : 0;
        default:
            return 1;
    }
}

void readKey() {
    auto scancode = Util::System::in.read();

    while (scancode != -1) {
        if (keyDecoder.parseScancode(scancode)) {
            const auto key = keyDecoder.getCurrentKey();

            switch (key.getScancode()) {
                case Util::Io::Key::W:
                    keys.up = key.isPressed();
                    break;
                case Util::Io::Key::S:
                    keys.down = key.isPressed();
                    break;
                case Util::Io::Key::A:
                    keys.left = key.isPressed();
                    break;
                case Util::Io::Key::D:
                    keys.right = key.isPressed();
                    break;
                case Util::Io::Key::K:
                    keys.a = key.isPressed();
                    break;
                case Util::Io::Key::J:
                    keys.b = key.isPressed();
                    break;
                case Util::Io::Key::ENTER:
                    keys.select = key.isPressed();
                    break;
                case Util::Io::Key::SPACE:
                    keys.start = key.isPressed();
                    break;
                case Util::Io::Key::ESC:
                    exit(0);
                    break;
                default:
                    break;
            }

            return;
        }

        scancode = Util::System::in.read();
    }
}

void wait_for_frame() {
    readKey();

    auto frameTime = Util::Time::Timestamp::getSystemTime() - lastTime;

    if (frameTime < targetFrameTime) {
        Util::Async::Thread::sleep(targetFrameTime - frameTime);
    }

    const auto currentTime = Util::Time::Timestamp::getSystemTime();
    frameTime = currentTime - lastTime;
    lastTime = currentTime;

    fpsCounter++;
    fpsTimer += frameTime;
    if (fpsTimer >= Util::Time::Timestamp::ofSeconds(1)) {
        fps = fpsCounter;
        fpsCounter = 0;
        fpsTimer = Util::Time::Timestamp();
    }
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("NES emulator by 'jiangyy' (https://github.com/NJU-ProjectN/LiteNES).\n"
                               "Joypad is mapped to WASD; A and B are mapped to K and J; Start is mapped to Space and Select is mapped to Enter.\n"
                               "Use 'F1' and 'F2' to adjust screen scaling. Use 'F3' to cycle through color palettes and 'F4' to reset to default palette.\n"
                               "Usage: peanut-gb [FILE]\n"
                               "Options:\n"
                               "  -s, --save: Path to save file\n"
                               "  -r, --resolution: Set display resolution\n"
                               "  -h, --help: Show this help message");

    argumentParser.addArgument("save", false, "s");
    argumentParser.addArgument("resolution", false, "r");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "litenes: No arguments provided!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto romFile = Util::Io::File(arguments[0]);
    auto stream = Util::Io::FileInputStream(romFile);

    auto *rom = new uint8_t[romFile.getLength()];
    stream.read(rom, 0, romFile.getLength());

    auto lfbFile = Util::Io::File("/device/lfb");

    if (argumentParser.hasArgument("resolution")) {
        auto split1 = argumentParser.getArgument("resolution").split("x");
        auto split2 = split1[1].split("@");

        auto resolutionX = Util::String::parseNumber<uint16_t>(split1[0]);
        auto resolutionY = Util::String::parseNumber<uint16_t>(split2[0]);
        uint8_t colorDepth = split2.length() > 1 ? Util::String::parseNumber<uint8_t>(split2[1]) : 32;

        lfbFile.controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
    }

    auto lfb = Util::Graphic::LinearFrameBuffer::open(lfbFile);
    auto doubleBuffer = Util::Graphic::BufferedLinearFrameBuffer(lfb);
    bufferedLfb = &doubleBuffer;
    scale = lfb.getResolutionX() / SCREEN_WIDTH > lfb.getResolutionY() / SCREEN_HEIGHT ? lfb.getResolutionY() / SCREEN_HEIGHT : lfb.getResolutionX() / SCREEN_WIDTH;
    drawResX = SCREEN_WIDTH * scale;
    drawResY = SCREEN_HEIGHT * scale;
    offsetX = lfb.getResolutionX() - SCREEN_WIDTH * scale > 0 ? (lfb.getResolutionX() - SCREEN_WIDTH * scale) / 2 : 0;
    offsetY = lfb.getResolutionY() - SCREEN_HEIGHT * scale > 0 ? (lfb.getResolutionY() - SCREEN_HEIGHT * scale) / 2 : 0;

    switch (lfb.getColorDepth()) {
        case 32:
            drawBackground = nes_set_bg_color32;
            flushBuf = nes_flush_buf32;
            break;
        case 24:
            drawBackground = nes_set_bg_color24;
            flushBuf = nes_flush_buf24;
            break;
        case 16:
        case 15:
            drawBackground = nes_set_bg_color16;
            flushBuf = nes_flush_buf16;
            break;
        default:
            Util::System::error << "litenes: Unsupported color depth!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
            return -1;
    }

    if (fce_load_rom(reinterpret_cast<char*>(rom)) != 0) {
        Util::System::error << "litenes: Invalid or unsupported rom!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    fce_init();
    fce_run();

    Util::Graphic::Ansi::cleanupGraphicalApplication();

    return 0;
}