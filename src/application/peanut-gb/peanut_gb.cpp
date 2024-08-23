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

#include "Peanut-GB/peanut_gb.h"
#include "lib/util/base/Exception.h"
#include "stdio.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/base/System.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/async/Thread.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/font/Terminal8x8.h"

const constexpr uint32_t TARGET_FRAME_RATE = 60;
const auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / TARGET_FRAME_RATE));

auto palette = new uint32_t[4] {
    Util::Graphic::Color(155, 188, 15).getRGB32(),
    Util::Graphic::Color(139, 172, 15).getRGB32(),
    Util::Graphic::Color(48, 98, 48).getRGB32(),
    Util::Graphic::Color(15, 56, 15).getRGB32()
};

uint8_t *rom = nullptr;
uint8_t *ram = nullptr;
uint8_t scale = 1;
uint8_t maxScale = 1;
uint16_t offsetX = 0;
uint16_t offsetY = 0;
Util::Graphic::LinearFrameBuffer *lfb = nullptr;

Util::Time::Timestamp fpsTimer;
uint32_t fpsCounter = 0;
uint32_t fps = 0;

uint8_t gb_rom_read(struct gb_s* gb, const uint_fast32_t addr) {
    return rom[addr];
}

uint8_t gb_cart_ram_read(struct gb_s* gb, const uint_fast32_t addr) {
    return ram[addr];
}

void gb_cart_ram_write(struct gb_s* gb, const uint_fast32_t addr, const uint8_t val) {
    ram[addr] = val;
}

void gb_error(struct gb_s* gb, const enum gb_error_e error, const uint16_t addr) {
    switch (error) {
        case GB_UNKNOWN_ERROR:
            printf("Unknown error at address 0x%04X\n", addr);
            break;
        case GB_INVALID_OPCODE:
            printf("Invalid opcode at address 0x%04X\n", addr);
            break;
        case GB_INVALID_READ:
            printf("Invalid read at address 0x%04X\n", addr);
            break;
        case GB_INVALID_WRITE:
            printf("Invalid write at address 0x%04X\n", addr);
            break;
        case GB_HALT_FOREVER:
            printf("Halting forever at address 0x%04X\n", addr);
            break;
        case GB_INVALID_MAX:
            printf("Invalid max at address 0x%04X\n", addr);
            break;
    }

    exit(error);
}

void lcd_draw_line(struct gb_s *gb, const uint8_t *pixels, const uint_fast8_t line) {
    auto screenBuffer = reinterpret_cast<uint32_t*>(lfb->getBuffer().add(offsetX * 4 + (offsetY + line * scale) * lfb->getPitch()).get());
    uint32_t resX = LCD_WIDTH * scale;

    for (uint32_t y = 0; y < scale; y++) {
        for (uint32_t x = 0; x < resX; x++) {
            uint8_t pixel = pixels[x / scale];
            auto color = palette[pixel & 0x03];
            screenBuffer[x] = color;
        }

        screenBuffer += (lfb->getPitch() / sizeof(uint32_t));
    }
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("GameBoy emulator by 'deltabeard' (https://github.com/deltabeard/Peanut-GB).\n"
                               "Arrow keys are mapped to WASD, A and B are mapped to K and J, Start is mapped to Space, Select is mapped to Enter. Use '+' and '-' to adjust screen scaling.\n"
                               "Usage: peanut-gb [FILE]...\n"
                               "Options:\n"
                               "  -r, --resolution: Set display resolution\n"
                               "  -h, --help: Show this help message");

    argumentParser.addArgument("resolution", false, "r");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "peanut-gb: No arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto file = Util::Io::File(arguments[0]);
    auto stream = Util::Io::FileInputStream(file);

    rom = new uint8_t[file.getLength()];
    stream.read(rom, 0, file.getLength());

    auto lfbFile = Util::Io::File("/device/lfb");

    if (argumentParser.hasArgument("resolution")) {
        auto split1 = argumentParser.getArgument("resolution").split("x");
        auto split2 = split1[1].split("@");

        uint32_t resolutionX = Util::String::parseInt(split1[0]);
        uint32_t resolutionY = Util::String::parseInt(split2[0]);
        uint32_t colorDepth = split2.length() > 1 ? Util::String::parseInt(split2[1]) : 32;

        lfbFile.controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
    }

    lfb = new Util::Graphic::LinearFrameBuffer(lfbFile);
    maxScale = lfb->getResolutionX() / LCD_WIDTH > lfb->getResolutionY() / LCD_HEIGHT ? lfb->getResolutionY() / LCD_HEIGHT : lfb->getResolutionX() / LCD_WIDTH;
    scale = maxScale;
    offsetX = lfb->getResolutionX() - LCD_WIDTH * scale > 0 ? (lfb->getResolutionX() - LCD_WIDTH * scale) / 2 : 0;
    offsetY = lfb->getResolutionY() - LCD_HEIGHT * scale > 0 ? (lfb->getResolutionY() - LCD_HEIGHT * scale) / 2 : 0;

    gb_s gb{};

    auto initResult = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, nullptr);
    if (initResult != GB_INIT_NO_ERROR) {
        Util::System::error << "peanut-gb: Failed to initialize emulator!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    ram = new uint8_t[gb_get_save_size(&gb)];

    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    gb_init_lcd(&gb, &lcd_draw_line);

    lfb->clear();

    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
    auto keyDecoder = Util::Io::KeyDecoder(new Util::Io::DeLayout());
    auto stringDrawer = Util::Graphic::StringDrawer(Util::Graphic::PixelDrawer(*lfb));

    while (true) {
        auto startTime = Util::Time::getSystemTime();

        auto c = Util::System::in.read();
        if (c != -1 && keyDecoder.parseScancode(c)) {
            auto key = keyDecoder.getCurrentKey();
            uint8_t joyKey = 0;

            switch (key.getScancode()) {
                case Util::Io::Key::UP:
                    joyKey = JOYPAD_UP;
                    break;
                case Util::Io::Key::LEFT:
                    joyKey = JOYPAD_LEFT;
                    break;
                case Util::Io::Key::DOWN:
                    joyKey = JOYPAD_DOWN;
                    break;
                case Util::Io::Key::RIGHT:
                    joyKey = JOYPAD_RIGHT;
                    break;
                case Util::Io::Key::K:
                    joyKey = JOYPAD_A;
                    break;
                case Util::Io::Key::J:
                    joyKey = JOYPAD_B;
                    break;
                case Util::Io::Key::SPACE:
                    joyKey = JOYPAD_START;
                    break;
                case Util::Io::Key::ENTER:
                    joyKey = JOYPAD_SELECT;
                    break;
                case Util::Io::Key::PLUS:
                    if (key.isPressed() && scale < maxScale) {
                        scale++;
                        offsetX = lfb->getResolutionX() - LCD_WIDTH * scale > 0 ? (lfb->getResolutionX() - LCD_WIDTH * scale) / 2 : 0;
                        offsetY = lfb->getResolutionY() - LCD_HEIGHT * scale > 0 ? (lfb->getResolutionY() - LCD_HEIGHT * scale) / 2 : 0;
                        lfb->clear();
                    }
                    break;
                case Util::Io::Key::MINUS:
                    if (key.isPressed() && scale > 1) {
                        scale--;
                        offsetX = lfb->getResolutionX() - LCD_WIDTH * scale > 0 ? (lfb->getResolutionX() - LCD_WIDTH * scale) / 2 : 0;
                        offsetY = lfb->getResolutionY() - LCD_HEIGHT * scale > 0 ? (lfb->getResolutionY() - LCD_HEIGHT * scale) / 2 : 0;
                        lfb->clear();
                    }
                    break;
                case Util::Io::Key::ESC:
                    return 0;
                default:
                    break;
            }

            if (key.isPressed()) {
                gb.direct.joypad &= ~joyKey;
            } else {
                gb.direct.joypad |= joyKey;
            }
        }

        gb_run_frame(&gb);
        stringDrawer.drawString(Util::Graphic::Fonts::TERMINAL_8x8, 0, 0, static_cast<const char*>(Util::String::format("FPS: %u", fps)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::BLACK);

        auto renderTime = Util::Time::getSystemTime() - startTime;
        if (renderTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - renderTime);
        }

        fpsCounter++;
        auto frameTime = Util::Time::getSystemTime() - startTime;
        fpsTimer += frameTime;

        if (fpsTimer >= Util::Time::Timestamp::ofSeconds(1)) {
            fps = fpsCounter;
            fpsCounter = 0;
            fpsTimer.reset();
        }
    }
}