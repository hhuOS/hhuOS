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
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

uint8_t audio_read(uint16_t addr);
void audio_write(uint16_t addr, uint8_t val);
#include "Peanut-GB/peanut_gb.h"

extern "C" {
#include "Peanut-GB/examples/sdl2/minigb_apu/minigb_apu.h"
}

#include "palettes.h"

#include <util/sound/AudioChannel.h>
#include <util/base/Panic.h>
#include <util/base/Address.h>
#include <util/base/ArgumentParser.h>
#include <util/base/System.h>
#include <util/io/file/File.h>
#include <util/io/stream/FileInputStream.h>
#include <util/graphic/LinearFrameBuffer.h>
#include <util/graphic/Colors.h>
#include <util/graphic/Ansi.h>
#include <util/time/Timestamp.h>
#include <util/async/Thread.h>
#include <util/io/key/KeyDecoder.h>
#include <util/io/key/layout/DeLayout.h>
#include <util/graphic/font/Terminal8x8.h>
#include <util/io/stream/FileOutputStream.h>
#include <util/base/String.h>
#include <util/collection/Array.h>
#include <util/io/key/KeyEvent.h>
#include <util/io/stream/InputStream.h>
#include <util/io/stream/PrintStream.h>

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

/// Frame rate that the emulator tries to run at.
constexpr size_t TARGET_FRAME_RATE = 60;
/// Time per frame if we want to hit the target frame rate.
constexpr auto TARGET_FRAME_TIME = Util::Time::Timestamp::ofSecondsFloat(1.0f / TARGET_FRAME_RATE);

/// The rom that is currently being played.
uint8_t *rom = nullptr;
/// The emulated cartridge ram (i.e., battery-backed storage for save games).
/// On exit, this buffer is written to a file.
/// If a corresponding file exists, it is loaded on start.
uint8_t *ram = nullptr;

/// The framebuffer to draw pixels to.
Util::Graphic::LinearFrameBuffer *lfb = nullptr;
/// The color palette currently used to draw pixels (Can be changed using F3 and reset using F4).
uint32_t *palette = nullptr;
/// The scale at which the screen is drawn (Can be changed using F1 and F2).
uint8_t scale = 1;
/// The maximum allowed scale. This is calculated at program start depending on the screen resolution.
uint8_t maxScale = 1;
/// The x-axis offset to draw the screen centered.
uint16_t offsetX = 0;
/// The y-axis offset to draw the screen centered.
uint16_t offsetY = 0;

/// The Game Boy instance, containing all emulation state.
gb_s gb;

/// The sound channel used to play audio from the emulator.
Util::Sound::AudioChannel audioChannel;
/// The audio processing unit instance.
minigb_apu_ctx apu;
/// Audio buffer for the `minigb_apu` library to write new samples to.
audio_sample_t minigbAudioBuffer[AUDIO_SAMPLES_TOTAL];
/// The APU writes 16-bit stereo samples at 22050 Hz.
/// However, the hhuOS audio channel expects 8-bit mono samples at 22050 Hz.
/// This buffer is used to mix the stereo sample from `minigb_apu` into 8-bit mono samples.
uint8_t mixedAudioBuffer[AUDIO_SAMPLES];

/// Timestamp used to count the number of frames per second (is reset every second).
Util::Time::Timestamp fpsTimer;
/// The number of frames that have been drawn since the last `fpsTimer` reset.
size_t fpsCounter = 0;
/// The number of frames counted during the last second. This is the number displayed on the screen.
size_t fps = 0;

/// Write the current cartridge RAM buffer to the specified file.
/// If the emulated game has no cartridge RAM, nothing is written.
void writeSaveFile(const Util::String &saveFilePath) {
    size_t saveSize;
    if (gb_get_save_size_s(&gb, &saveSize) != 0) {
        return;
    }

    const Util::Io::File saveFile(saveFilePath);
    if (!saveFile.exists()) {
        if (!saveFile.create(Util::Io::File::REGULAR)) {
            Util::System::error << "peanut-gb: Failed to create save file!" << Util::Io::PrintStream::lnFlush;
            return;
        }
    }

    Util::Io::FileOutputStream(saveFile).write(ram, 0, saveSize);
}

/// Read the specified file into the cartridge RAM buffer.
/// The buffer is allocated according to the emulated game.
/// If the game has no cartridge RAM, nothing is read/allocated.
void readSaveFile(const Util::String &saveFilePath) {
    size_t saveSize;
    if (gb_get_save_size_s(&gb, &saveSize) != 0) {
        return;
    }

    ram = new uint8_t[saveSize];

    const Util::Io::File saveFile(saveFilePath);
    if (saveFile.exists()) {
        Util::Io::FileInputStream(saveFile).read(ram, 0, saveSize);
    }
}

/// LCD drawing function for 32-bit framebuffers.
/// A pointer to this function is given to Peanut-GB using `gb_init_lcd()` during program start.
/// Peanut-GB calls this function directly for each line of pixels.
void drawLine32(gb_s*, const uint8_t *pixels, const uint_fast8_t line) {
    const uint16_t resX = LCD_WIDTH * scale;
    auto screenBuffer = reinterpret_cast<uint32_t*>(
        lfb->getBuffer().add(offsetX * 4 + (offsetY + line * scale) * lfb->getPitch()).get());

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            const auto pixel = pixels[x / scale];
            const auto color = palette[(pixel >> 2) | (pixel & 0x03)];

            screenBuffer[x] = color;
        }

        screenBuffer += lfb->getPitch() / 4;
    }
}

/// LCD drawing function for 24-bit framebuffers.
/// A pointer to this function is given to Peanut-GB using `gb_init_lcd()` during program start.
/// Peanut-GB calls this function directly for each line of pixels.
void drawLine24(gb_s*, const uint8_t *pixels, const uint_fast8_t line) {
    const uint16_t resX = LCD_WIDTH * scale;
    auto screenBuffer = reinterpret_cast<uint8_t*>(
        lfb->getBuffer().add(offsetX * 3 + (offsetY + line * scale) * lfb->getPitch()).get());

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            const auto pixel = pixels[x / scale];
            const auto color = palette[(pixel >> 2) | (pixel & 0x03)];

            screenBuffer[x * 3] = color & 0xff;
            screenBuffer[x * 3 + 1] = (color >> 8) & 0xff;
            screenBuffer[x * 3 + 2] = (color >> 16) & 0xff;
        }

        screenBuffer += lfb->getPitch();
    }
}

/// LCD drawing function for 15/16-bit framebuffers.
/// A pointer to this function is given to Peanut-GB using `gb_init_lcd()` during program start.
/// Peanut-GB calls this function directly for each line of pixels.
void drawLine16(gb_s*, const uint8_t *pixels, const uint_fast8_t line) {
    const uint16_t resX = LCD_WIDTH * scale;
    auto screenBuffer = reinterpret_cast<uint16_t*>(
        lfb->getBuffer().add(offsetX * 2 + (offsetY + line * scale) * lfb->getPitch()).get());

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            const auto pixel = pixels[x / scale];
            const auto color = palette[(pixel >> 2) | (pixel & 0x03)];

            screenBuffer[x] = color;
        }

        screenBuffer += lfb->getPitch() / 2;
    }
}

/// Read a byte from the ROM buffer.
/// This declared by Peanut-GB and must be implemented by the frontend.
uint8_t gb_rom_read(gb_s*, const uint_fast32_t addr) {
    return rom[addr];
}

/// Read a byte from the cartridge RAM buffer.
/// This function is declared by Peanut-GB and must be implemented by the frontend.
uint8_t gb_cart_ram_read(gb_s*, const uint_fast32_t addr) {
    return ram[addr];
}

/// Write a byte from the cartridge RAM buffer.
/// This function is declared by Peanut-GB and must be implemented by the frontend.
void gb_cart_ram_write(gb_s*, const uint_fast32_t addr, const uint8_t val) {
    ram[addr] = val;
}

/// Handle emulation errors.
/// This function is declared by Peanut-GB and must be implemented by the frontend.
void gb_error(gb_s*, const gb_error_e error, const uint16_t addr) {
    Util::System::error << "peanut-gb: ";

    switch (error) {
        case GB_INVALID_OPCODE:
            Util::System::error << "Invalid opcode";
            break;
        case GB_INVALID_READ:
            Util::System::error << "Invalid read";
            break;
        case GB_INVALID_WRITE:
            Util::System::error << "Invalid write";
            break;
        case GB_INVALID_MAX:
            Util::System::error << "Invalid max";
            break;
        default:
            Util::System::error << "Unknown error";
            break;
    }

    Util::System::error << " at address 0x" << Util::Io::PrintStream::hex << addr << Util::Io::PrintStream::lnFlush;
    exit(error);
}

/// Read a byte from the audio processing unit.
/// If audio is enabled during compilation, the frontend must implement this function.
/// We simply pass the call over to `minigb_apu`.
uint8_t audio_read(const uint16_t addr) {
    return minigb_apu_audio_read(&apu, addr);
}

/// Write a byte to the audio processing unit.
/// If audio is enabled during compilation, the frontend must implement this function.
/// We simply pass the call over to `minigb_apu`.
void audio_write(const uint16_t addr, const uint8_t val) {
    minigb_apu_audio_write(&apu, addr, val);
}

int32_t main(const int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_TEXT);

    argumentParser.addArgument("save", false, "s");
    argumentParser.addArgument("resolution", false, "r");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "peanut-gb: No arguments provided!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    // Read the ROM file
    const Util::Io::File romFile(arguments[0]);
    Util::Io::FileInputStream stream(romFile);

    rom = new uint8_t[romFile.getLength()];
    stream.read(rom, 0, romFile.getLength());

    // Initialize the linear frame buffer and scale/offset variables
    auto lfbFile = Util::Io::File("/device/lfb");
    if (argumentParser.hasArgument("resolution")) {
        const auto resolutionString = argumentParser.getArgument("resolution");
        Util::Graphic::LinearFrameBuffer::setResolution(lfbFile, resolutionString);
    }

    Util::Graphic::LinearFrameBuffer framebuffer(lfbFile);
    lfb = &framebuffer;
    maxScale = lfb->getResolutionX() / LCD_WIDTH > lfb->getResolutionY() / LCD_HEIGHT ?
        lfb->getResolutionY() / LCD_HEIGHT : lfb->getResolutionX() / LCD_WIDTH;
    scale = maxScale;
    offsetX = lfb->getResolutionX() - LCD_WIDTH * scale > 0 ? (lfb->getResolutionX() - LCD_WIDTH * scale) / 2 : 0;
    offsetY = lfb->getResolutionY() - LCD_HEIGHT * scale > 0 ? (lfb->getResolutionY() - LCD_HEIGHT * scale) / 2 : 0;

    // Initialize the emulator instance
    auto initResult = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, nullptr);
    if (initResult != GB_INIT_NO_ERROR) {
        Util::System::error << "peanut-gb: Failed to initialize emulator!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    // Read the save file (if existing or passed as program parameter)
    Util::String saveFilePath;
    if (argumentParser.hasArgument("save")) {
        saveFilePath = argumentParser.getArgument("save");
    } else {
        char title[16]{};
        gb_get_rom_name(&gb, title);

        saveFilePath = romFile.getParent().getCanonicalPath() + "/" + Util::String(title).strip() + ".sav";
    }

    readSaveFile(saveFilePath);

    // Initialize Peanut-GB LCD support
    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    switch (lfb->getColorDepth()) {
        case 32:
            gb_init_lcd(&gb, &drawLine32);
            break;
        case 24:
            gb_init_lcd(&gb, &drawLine24);
            break;
        case 15:
        case 16:
            gb_init_lcd(&gb, &drawLine16);
            break;
        default:
            Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION, "peanut-gb: Unsupported color depth!");
    }

    lfb->clear();

    uint8_t manualPaletteIndex = UINT8_MAX;
    palette = getPalette(&gb, lfb->getColorDepth());

    // Initialize Peanut-GB real-time clock support
    auto timer = time(nullptr);
    auto date = localtime(&timer);
    gb_set_rtc(&gb, date);

    // Initialize `minigb_apu` for audio support
    minigb_apu_audio_init(&apu);
    audioChannel.play();

    // Initialize keyboard input
    Util::Io::DeLayout layout;
    Util::Io::KeyDecoder keyDecoder(layout);
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    // Enter main loop
    while (true) {
        // Get time for frame time measurement
        auto startTime = Util::Time::Timestamp::getSystemTime();

        // Read and process all available key events
        auto c = Util::System::in.read();
        while (c != -1 && keyDecoder.parseScancode(c)) {
            auto key = keyDecoder.getKeyEvent();
            uint8_t joyKey = 0;

            switch (key.getScancode()) {
                case Util::Io::KeyEvent::UP:
                    joyKey = JOYPAD_UP;
                    break;
                case Util::Io::KeyEvent::LEFT:
                    joyKey = JOYPAD_LEFT;
                    break;
                case Util::Io::KeyEvent::DOWN:
                    joyKey = JOYPAD_DOWN;
                    break;
                case Util::Io::KeyEvent::RIGHT:
                    joyKey = JOYPAD_RIGHT;
                    break;
                case Util::Io::KeyEvent::K:
                    joyKey = JOYPAD_A;
                    break;
                case Util::Io::KeyEvent::J:
                    joyKey = JOYPAD_B;
                    break;
                case Util::Io::KeyEvent::SPACE:
                    joyKey = JOYPAD_START;
                    break;
                case Util::Io::KeyEvent::ENTER:
                    joyKey = JOYPAD_SELECT;
                    break;
                case Util::Io::KeyEvent::F1: // Increase screen scale
                    if (key.isPressed() && scale < maxScale) {
                        scale++;
                        offsetX = lfb->getResolutionX() - LCD_WIDTH * scale > 0 ?
                            (lfb->getResolutionX() - LCD_WIDTH * scale) / 2 : 0;
                        offsetY = lfb->getResolutionY() - LCD_HEIGHT * scale > 0 ?
                            (lfb->getResolutionY() - LCD_HEIGHT * scale) / 2 : 0;
                        lfb->clear();
                    }
                    break;
                case Util::Io::KeyEvent::F2: // Decrease screen scale
                    if (key.isPressed() && scale > 1) {
                        scale--;
                        offsetX = lfb->getResolutionX() - LCD_WIDTH * scale > 0 ?
                            (lfb->getResolutionX() - LCD_WIDTH * scale) / 2 : 0;
                        offsetY = lfb->getResolutionY() - LCD_HEIGHT * scale > 0 ?
                            (lfb->getResolutionY() - LCD_HEIGHT * scale) / 2 : 0;
                        lfb->clear();
                    }
                    break;
                case Util::Io::KeyEvent::F3: // Cycle color palettes
                    if (key.isPressed()) {
                        manualPaletteIndex = manualPaletteIndex + 1 > 14 ? 0 : manualPaletteIndex + 1;

                        delete palette;
                        palette = getManualPalette(manualPaletteIndex, lfb->getColorDepth());
                    }
                    break;
                case Util::Io::KeyEvent::F4: // Reset to original color palette
                    if (key.isPressed() && manualPaletteIndex != UINT8_MAX) {
                        manualPaletteIndex = UINT8_MAX;

                        delete palette;
                        palette = getPalette(&gb, lfb->getColorDepth());
                    }
                    break;
                case Util::Io::KeyEvent::ESC: // Exit the emulator
                    writeSaveFile(saveFilePath);
                    return 0;
                default:
                    break;
            }

            if (key.isPressed()) {
                gb.direct.joypad &= ~joyKey;
            } else {
                gb.direct.joypad |= joyKey;
            }

            c = Util::System::in.read();
        }

        // Emulate the next frame -> Automatically calls LCD draw function to update the screen
        gb_run_frame(&gb);

        // Get signed 16-bit stereo audio samples from the APU
        minigb_apu_audio_callback(&apu, minigbAudioBuffer);

        // Convert the samples to unsigned 8-bit mono samples
        for (size_t i = 0; i < AUDIO_SAMPLES; i++) {
            // Convert the signed 16-bit samples to unsigned 8-bit
            auto sample1 = (minigbAudioBuffer[i * 2] >> 8) + INT8_MAX; // First channel
            auto sample2 = (minigbAudioBuffer[i * 2 + 1] >> 8) + INT8_MAX; // Second channel

            // Mix the two channels and clamp the value to the range of 0-255
            auto mixed = (sample1 + sample2) / 2;
            mixedAudioBuffer[i] = mixed > UINT8_MAX ? UINT8_MAX : mixed;
        }

        // Play the mixed audio samples
        audioChannel.write(mixedAudioBuffer, 0, AUDIO_SAMPLES);

        lfb->drawString(Util::Graphic::Fonts::TERMINAL_8x8, 0, 0, static_cast<const char*>(Util::String::format("FPS: %02u", fps)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::BLACK);
        // Update FPS display
        lfb->drawString(Util::Graphic::Fonts::TERMINAL_8x8, 0, 0,
            static_cast<const char*>(Util::String::format("FPS: %u", fps)),
            Util::Graphic::Colors::WHITE, Util::Graphic::Colors::BLACK);

        // Sleep to hit the target framerate
        auto renderTime = Util::Time::Timestamp::getSystemTime() - startTime;
        if (renderTime < TARGET_FRAME_TIME) {
            Util::Async::Thread::sleep(TARGET_FRAME_TIME - renderTime);
        }

        // Update FPS counter
        fpsCounter++;
        auto frameTime = Util::Time::Timestamp::getSystemTime() - startTime;
        fpsTimer += frameTime;

        if (fpsTimer >= Util::Time::Timestamp::ofSeconds(1)) {
            fps = fpsCounter;
            fpsCounter = 0;
            fpsTimer = Util::Time::Timestamp();
        }
    }
}