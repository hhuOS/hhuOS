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

#include <stdint.h>

#include "lib/util/base/System.h"
#include "lib/util/io/stream/PrintStream.h"
#include "application/clownmdemu/clownmdemu/clowncommon/clowncommon.h"
#include "application/clownmdemu/clownmdemu/clownmdemu.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/async/Thread.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/font/Terminal8x8.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/io/key/KeyDecoder.h"

const ClownMDEmu_Constant constants = ClownMDEmu_Constant_Initialise();
ClownMDEmu emulator{};
ClownMDEmu_State state{};
ClownMDEmu_Configuration configuration{};

uint8_t *rom = nullptr;
bool buttons[CLOWNMDEMU_BUTTON_MAX]{};

uint32_t palette[65536]{};
Util::Graphic::LinearFrameBuffer *lfb = nullptr;

uint32_t targetFrameRate = 0;
auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(0);

cc_u16f mdScreenWidth = 0;
cc_u16f mdScreenHeight = 0;
uint8_t maxScale = 0;
uint8_t scale = 0;
uint16_t offsetX = 0;
uint16_t offsetY = 0;
uint16_t resX = 0;

Util::Time::Timestamp fpsTimer;
uint32_t fpsCounter = 0;
uint32_t fps = 0;

void calculateScreenVariables(cc_u16f screen_width, cc_u16f screen_height, bool recalculate = false) {
    if (mdScreenWidth == screen_width && mdScreenHeight == screen_height && !recalculate) {
        return;
    }

    mdScreenWidth = screen_width;
    mdScreenHeight = screen_height;

    maxScale = lfb->getResolutionX() / screen_width > lfb->getResolutionY() / screen_height ? lfb->getResolutionY() / screen_height : lfb->getResolutionX() / screen_width;
    if (maxScale == 0) {
        maxScale = 1;
    }
    if (scale == 0 || scale > maxScale) {
        scale = maxScale;
    }

    offsetX = lfb->getResolutionX() - screen_width * scale > 0 ? (lfb->getResolutionX() - screen_width * scale) / 2 : 0;
    offsetY = static_cast<int16_t>(lfb->getResolutionY() - screen_height * scale) > 0 ? (lfb->getResolutionY() - screen_height * scale) / 2 : 0;
    resX = screen_width * scale;

    lfb->clear();
}

void LogCallback([[maybe_unused]] void *user_data, [[maybe_unused]] const char *format, [[maybe_unused]] va_list args) {}

cc_u8f CartridgeReadCallback([[maybe_unused]] void *user_data, cc_u32f address) {
    return rom[address];
}

void CartridgeWrittenCallback([[maybe_unused]] void *user_data, [[maybe_unused]] cc_u32f address, [[maybe_unused]] cc_u8f value) {}

void ColourUpdatedCallback([[maybe_unused]] void *user_data, cc_u16f index, cc_u16f colour) {
    // Decompose XBGR4444 into individual colour channels
    auto red = (colour >> 4 * 0) & 0xf;
    auto green = (colour >> 4 * 1) & 0xf;
    auto blue = (colour >> 4 * 2) & 0xf;

    palette[index] = Util::Graphic::Color(red << 4 | red, green << 4 | green, blue << 4 | blue).getColorForDepth(lfb->getColorDepth());
}

void ScanlineRenderedCallback32([[maybe_unused]] void *user_data, cc_u16f scanline, const cc_u8l *pixels, cc_u16f screen_width, cc_u16f screen_height) {
    calculateScreenVariables(screen_width, screen_height);
    auto *screenBuffer = reinterpret_cast<uint32_t*>(lfb->getBuffer().add(offsetX * 4 + (offsetY + scanline * scale) * lfb->getPitch()).get());

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            uint8_t pixel = pixels[x / scale];
            uint32_t color = palette[pixel];

            screenBuffer[x] = color;
        }

        screenBuffer += (lfb->getPitch() / 4);
    }
}

void ScanlineRenderedCallback24([[maybe_unused]] void *user_data, cc_u16f scanline, const cc_u8l *pixels, cc_u16f screen_width, cc_u16f screen_height) {
    calculateScreenVariables(screen_width, screen_height);
    auto *screenBuffer = reinterpret_cast<uint8_t*>(lfb->getBuffer().add(offsetX * 3 + (offsetY + scanline * scale) * lfb->getPitch()).get());

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            uint8_t pixel = pixels[x / scale];
            uint32_t color = palette[pixel];

            screenBuffer[x * 3] = color & 0xff;
            screenBuffer[x * 3 + 1] = (color >> 8) & 0xff;
            screenBuffer[x * 3 + 2] = (color >> 16) & 0xff;
        }

        screenBuffer += (lfb->getPitch());
    }
}

void ScanlineRenderedCallback16([[maybe_unused]] void *user_data, cc_u16f scanline, const cc_u8l *pixels, cc_u16f screen_width, cc_u16f screen_height) {
    calculateScreenVariables(screen_width, screen_height);
    auto *screenBuffer = reinterpret_cast<uint16_t*>(lfb->getBuffer().add(offsetX * 2 + (offsetY + scanline * scale) * lfb->getPitch()).get());

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            uint8_t pixel = pixels[x / scale];
            uint16_t color = palette[pixel];

            screenBuffer[x] = color;
        }

        screenBuffer += (lfb->getPitch() / 2);
    }
}

cc_bool ReadInputCallback([[maybe_unused]] void *user_data, [[maybe_unused]] cc_u8f player_id, ClownMDEmu_Button button_id) {
    return buttons[button_id];
}

void FMAudioCallback([[maybe_unused]] void *user_data, [[maybe_unused]] const ClownMDEmu *clownmdemu, [[maybe_unused]] std::size_t total_frames, [[maybe_unused]] void (*generate_fm_audio)(const ClownMDEmu *clownmdemu, cc_s16l *sample_buffer, std::size_t total_frames)) {}

void PSGAudioCallback([[maybe_unused]] void *user_data, [[maybe_unused]] const ClownMDEmu *clownmdemu, [[maybe_unused]] std::size_t total_samples, [[maybe_unused]] void (*generate_psg_audio)(const ClownMDEmu *clownmdemu, cc_s16l *sample_buffer, std::size_t total_samples)) {}

void PCMAudioCallback([[maybe_unused]] void *user_data, [[maybe_unused]] const ClownMDEmu *clownmdemu, [[maybe_unused]] std::size_t total_frames, [[maybe_unused]] void (*generate_pcm_audio)(const ClownMDEmu *clownmdemu, cc_s16l *sample_buffer, std::size_t total_frames)) {}

void CDDAAudioCallback([[maybe_unused]] void *user_data, [[maybe_unused]] const ClownMDEmu *clownmdemu, [[maybe_unused]] std::size_t total_frames, [[maybe_unused]] void (*generate_cdda_audio)(const ClownMDEmu *clownmdemu, cc_s16l *sample_buffer, std::size_t total_frames)) {}

void CDSeekCallback([[maybe_unused]] void *user_data, [[maybe_unused]] cc_u32f sector_index) {}

const cc_u8l* CDSectorReadCallback([[maybe_unused]] void *user_data) {
    return nullptr;
}

cc_bool CDSeekTrackCallback([[maybe_unused]] void *user_data, [[maybe_unused]] cc_u16f track_index, [[maybe_unused]] ClownMDEmu_CDDAMode mode) {
    return false;
}

std::size_t CDAudioReadCallback([[maybe_unused]] void *user_data, [[maybe_unused]] cc_s16l *sample_buffer, [[maybe_unused]] std::size_t total_frames) {
    return 0;
}

ClownMDEmu_Callbacks callbacks{};

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Mega Drive emulator by 'Clownacy' (https://github.com/Clownacy/clownmdemu).\n"
                               "Joypad is mapped to arrow keys; ABC and XYZ buttons are mapped to A, S, D and Y(Z), X, C respectively; Start is mapped to Space and Mode is mapped to Enter.\n"
                               "Use 'F1' and 'F2' to adjust screen scaling.\n"
                               "Usage: clownmdemu [FILE]\n"
                               "Options:\n"
                               "  -r, --resolution: Set display resolution\n"
                               "  -h, --help: Show this help message");

    argumentParser.addArgument("save", false, "s");
    argumentParser.addArgument("resolution", false, "r");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "clownmdemu: No arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto romFile = Util::Io::File(arguments[0]);
    auto stream = Util::Io::FileInputStream(romFile);

    rom = new uint8_t[romFile.getLength()];
    stream.read(rom, 0, romFile.getLength());

    auto lfbFile = Util::Io::File("/device/lfb");

    if (argumentParser.hasArgument("resolution")) {
        auto split1 = argumentParser.getArgument("resolution").split("x");
        auto split2 = split1[1].split("@");

        uint32_t resolutionX = Util::String::parseInt(split1[0]);
        uint32_t resolutionY = Util::String::parseInt(split2[0]);
        uint32_t colorDepth = split2.length() > 1 ? Util::String::parseInt(split2[1]) : 32;

        lfbFile.controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
    }

    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    lfb = new Util::Graphic::LinearFrameBuffer(lfbFile);
    lfb->clear();

    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
    auto keyDecoder = Util::Io::KeyDecoder(new Util::Io::DeLayout());
    auto stringDrawer = Util::Graphic::StringDrawer(Util::Graphic::PixelDrawer(*lfb));

    auto scanlineCallback = ScanlineRenderedCallback32;
    if (lfb->getColorDepth() == 24) {
        scanlineCallback = ScanlineRenderedCallback24;
    } else if (lfb->getColorDepth() == 16) {
        scanlineCallback = ScanlineRenderedCallback16;
    }

    callbacks = {
        nullptr,
                CartridgeReadCallback,
                CartridgeWrittenCallback,
                ColourUpdatedCallback,
                scanlineCallback,
                ReadInputCallback,
                FMAudioCallback,
                PSGAudioCallback,
                PCMAudioCallback,
                CDDAAudioCallback,
                CDSeekCallback,
                CDSectorReadCallback,
                CDSeekTrackCallback,
                CDAudioReadCallback
    };

    configuration.general.region = CLOWNMDEMU_REGION_OVERSEAS;
    configuration.general.tv_standard = CLOWNMDEMU_TV_STANDARD_NTSC;

    targetFrameRate = configuration.general.tv_standard == CLOWNMDEMU_TV_STANDARD_NTSC ? 60 : 50;
    targetFrameTime = Util::Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / targetFrameRate));

    ClownMDEmu_SetLogCallback(LogCallback, nullptr);
    ClownMDEmu_State_Initialise(&state);
    ClownMDEmu_Parameters_Initialise(&emulator, &configuration, &constants, &state, &callbacks);
    ClownMDEmu_Reset(&emulator, false);

    while(true) {
        auto startTime = Util::Time::getSystemTime();

        auto c = Util::System::in.read();
        if (c != -1 && keyDecoder.parseScancode(c)) {
            bool pressed = keyDecoder.getCurrentKey().isPressed();

            switch (keyDecoder.getCurrentKey().getScancode()) {
                case Util::Io::Key::UP:
                    buttons[CLOWNMDEMU_BUTTON_UP] = pressed;
                    break;
                case Util::Io::Key::DOWN:
                    buttons[CLOWNMDEMU_BUTTON_DOWN] = pressed;
                    break;
                case Util::Io::Key::LEFT:
                    buttons[CLOWNMDEMU_BUTTON_LEFT] = pressed;
                    break;
                case Util::Io::Key::RIGHT:
                    buttons[CLOWNMDEMU_BUTTON_RIGHT] = pressed;
                    break;
                case Util::Io::Key::A:
                    buttons[CLOWNMDEMU_BUTTON_A] = pressed;
                    break;
                case Util::Io::Key::S:
                    buttons[CLOWNMDEMU_BUTTON_B] = pressed;
                    break;
                case Util::Io::Key::D:
                    buttons[CLOWNMDEMU_BUTTON_C] = pressed;
                    break;
                case Util::Io::Key::Y:
                    buttons[CLOWNMDEMU_BUTTON_X] = pressed;
                    break;
                case Util::Io::Key::X:
                    buttons[CLOWNMDEMU_BUTTON_Y] = pressed;
                    break;
                case Util::Io::Key::C:
                    buttons[CLOWNMDEMU_BUTTON_Z] = pressed;
                    break;
                case Util::Io::Key::SPACE:
                    buttons[CLOWNMDEMU_BUTTON_START] = pressed;
                    break;
                case Util::Io::Key::ENTER:
                    buttons[CLOWNMDEMU_BUTTON_MODE] = pressed;
                    break;
                case Util::Io::Key::F1:
                    if (scale > 1) {
                        scale--;
                        calculateScreenVariables(mdScreenWidth, mdScreenHeight, true);
                    }
                    break;
                case Util::Io::Key::F2:
                    if (scale < maxScale) {
                        scale++;
                        calculateScreenVariables(mdScreenWidth, mdScreenHeight, true);
                    }
                    break;
                case Util::Io::Key::ESC:
                    return 0;
            }
        }

        ClownMDEmu_Iterate(&emulator);

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