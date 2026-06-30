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

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "clownmdemu/libraries/clowncommon/clowncommon.h"
#include "clownmdemu/source/clownmdemu.h"

#include <util/async/Thread.h>
#include <util/base/Address.h>
#include <util/base/String.h>
#include <util/base/ArgumentParser.h>
#include <util/base/System.h>
#include <util/collection/Array.h>
#include <util/io/file/File.h>
#include <util/io/stream/InputStream.h>
#include <util/io/stream/FileOutputStream.h>
#include <util/io/stream/FileInputStream.h>
#include <util/io/stream/PrintStream.h>
#include <util/io/key/layout/DeLayout.h>
#include <util/io/key/KeyDecoder.h>
#include <util/io/key/KeyEvent.h>
#include <util/graphic/LinearFrameBuffer.h>
#include <util/graphic/font/Terminal8x8.h>
#include <util/graphic/Colors.h>
#include <util/graphic/Ansi.h>
#include <util/graphic/Color.h>
#include <util/time/Timestamp.h>

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

/// The emulator instance.
ClownMDEmu emulator;
/// The state of the emulator.
ClownMDEmu_State state;
/// The emulator configuration struct.
ClownMDEmu_InitialConfiguration configuration;
/// Contains all callbacks for the emulator that are implemented by this frontend.
ClownMDEmu_Callbacks callbacks;

/// An array of boolean values for every controller button.
/// True means the button is currently pressed; false means it is not.
bool buttons[CLOWNMDEMU_BUTTON_MAX];

/// The color palette used to render pixels on the screen.
/// The colors are set by the `colorUpdated()` callback.
uint32_t palette[65536];
/// The framebuffer to draw pixels to.
Util::Graphic::LinearFrameBuffer *lfb = nullptr;

/// Frame rate that the emulator tries to run at.
/// The value depends on the region (50 for PAL, 60 for NTSC).
size_t targetFrameRate = 0;
/// Time per frame if we want to hit the target frame rate.
Util::Time::Timestamp targetFrameTime;

/// The file to write savegames to.
/// It is opened/created by the `saveFileOpenedForReading()`/`saveFileOpenedForWriting()` callbacks.
Util::Io::File saveFile;
/// Input stream for the savegame file.
/// It is read by the `saveFileRead()` callback.
Util::Io::FileInputStream *saveFileInputStream = nullptr;
/// Output stream for the savegame file.
/// It is written by the `saveFileWritten()` callback.
Util::Io::FileOutputStream *saveFileOutputStream = nullptr;

/// The resolution width at which the game is rendered.
/// The value depends on the region (PAL/NTSC) and is set the first time a scanline is rendered.
cc_u16f mdScreenWidth = 0;
/// The resolution height at which the game is rendered.
/// The value depends on the region (PAL/NTSC) and is set the first time a scanline is rendered.
cc_u16f mdScreenHeight = 0;
/// The maximum allowed scale. This is calculated after `mdScreenWidth` and `mdScreenHeight` have been set.
uint8_t maxScale = 0;
/// The scale at which the screen is drawn (Can be changed using F1 and F2).
uint8_t scale = 0;
/// The x-axis offset to draw the screen centered.
uint16_t offsetX = 0;
/// The y-axis offset to draw the screen centered.
uint16_t offsetY = 0;

/// Timestamp used to count the number of frames per second (is reset every second).
Util::Time::Timestamp fpsTimer;
/// The number of frames that have been drawn since the last `fpsTimer` reset.
size_t fpsCounter = 0;
/// The number of frames counted during the last second. This is the number displayed on the screen.
size_t fps = 0;

/// Calculate the global screen variables (e.g., scale, offset, etc.) based on the game render resolution.
/// If the given `width` and `height` are equal to `mdScreenWidth` and `mdScreenHeight`, the function
/// directly returns without doing anything. Setting `recalculate` to true forces recalculation of all variables.
void updateScreenVariables(const cc_u16f width, const cc_u16f height, const bool recalculate = false) {
    if (mdScreenWidth == width && mdScreenHeight == height && !recalculate) {
        return;
    }

    const auto lfbWidth = lfb->getResolutionX();
    const auto lfbHeight = lfb->getResolutionY();

    mdScreenWidth = width;
    mdScreenHeight = height;

    maxScale = lfbWidth / width > lfbHeight / height ? lfbHeight / height : lfbWidth / width;

    if (maxScale == 0) {
        maxScale = 1;
    }
    if (scale == 0 || scale > maxScale) {
        scale = maxScale;
    }

    offsetX = lfbWidth - width * scale > 0 ? (lfbWidth - width * scale) / 2 : 0;
    offsetY = lfbHeight - height * scale > 0 ? (lfbHeight - height * scale) / 2 : 0;

    lfb->clear();
}

/// Callback function that writes log messages to standard out.
void log(void*, const char *format, va_list args) {
    const auto message = Util::String::format(format, args);
    Util::System::out << message << Util::Io::PrintStream::lnFlush;
}

/// Callback function that updates a color in `palette`.
void colorUpdated(void*, const cc_u16f index, const cc_u16f color) {
    // Decompose XBGR4444 into individual colour channels
    const auto red = (color >> 4 * 0) & 0xf;
    const auto green = (color >> 4 * 1) & 0xf;
    const auto blue = (color >> 4 * 2) & 0xf;

    const auto newColor = Util::Graphic::Color(red << 4 | red, green << 4 | green, blue << 4 | blue);
    palette[index] = newColor.getColorForDepth(lfb->getColorDepth());
}

/// Callback function that draws a scanline from the emulated console to the screen (32-bit color variant).
void scanlineRendered32(void*, const cc_u16f scanline, const cc_u8l *pixels, cc_u16f, cc_u16f,
    const cc_u16f screenWidth, const cc_u16f screenHeight)
{
    updateScreenVariables(screenWidth, screenHeight);
    const auto resX = mdScreenWidth * scale;
    const auto screenAddr = lfb->getBuffer().add(offsetX * 4 + (offsetY + scanline * scale) * lfb->getPitch());
    auto *screenBuffer = reinterpret_cast<uint32_t*>(screenAddr.get());

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            const uint8_t pixel = pixels[x / scale];
            const uint32_t color = palette[pixel];

            screenBuffer[x] = color;
        }

        screenBuffer += lfb->getPitch() / 4;
    }
}

/// Callback function that draws a scanline from the emulated console to the screen (24-bit color variant).
void scanlineRendered24(void*, const cc_u16f scanline, const cc_u8l *pixels, cc_u16f, cc_u16f,
    const cc_u16f screenWidth, const cc_u16f screenHeight)
{
    updateScreenVariables(screenWidth, screenHeight);
    const auto resX = mdScreenWidth * scale;
    const auto screenAddr = lfb->getBuffer().add(offsetX * 3 + (offsetY + scanline * scale) * lfb->getPitch());
    auto *screenBuffer = reinterpret_cast<uint8_t*>(screenAddr.get());

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            const uint8_t pixel = pixels[x / scale];
            const uint32_t color = palette[pixel];

            screenBuffer[x * 3] = color & 0xff;
            screenBuffer[x * 3 + 1] = (color >> 8) & 0xff;
            screenBuffer[x * 3 + 2] = (color >> 16) & 0xff;
        }

        screenBuffer += lfb->getPitch();
    }
}

/// Callback function that draws a scanline from the emulated console to the screen (16-bit color variant).
void scanlineRendered16(void*, const cc_u16f scanline, const cc_u8l *pixels, cc_u16f, cc_u16f,
    const cc_u16f screenWidth, const cc_u16f screenHeight)
{
    updateScreenVariables(screenWidth, screenHeight);
    const auto resX = mdScreenWidth * scale;
    const auto screenAddr = lfb->getBuffer().add(offsetX * 2 + (offsetY + scanline * scale) * lfb->getPitch());
    auto *screenBuffer = reinterpret_cast<uint16_t*>(screenAddr.get());

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            const uint8_t pixel = pixels[x / scale];
            const uint16_t color = palette[pixel];

            screenBuffer[x] = color;
        }

        screenBuffer += lfb->getPitch() / 2;
    }
}

/// Callback function that reads the state (pressed/released) of a controller button.
cc_bool inputRequested(void*, cc_u8f, const ClownMDEmu_Button buttonId) {
    return buttons[buttonId];
}

/// Callback function that handles FM audio playback (not supported by this frontend).
void fmAudioToBeGenerated(void*, ClownMDEmu*, size_t,
    void (*)(ClownMDEmu *clownmdemu, cc_s16l *sampleBuffer, size_t totalFrames)) {}

/// Callback function that handles PSG audio playback (not supported by this frontend).
void psgAudioToBeGenerated(void*, ClownMDEmu*, size_t,
    void (*)(ClownMDEmu *clownmdemu, cc_s16l *sampleBuffer, size_t totalSamples)) {}

/// Callback function that handles PCM audio playback (not supported by this frontend).
void pcmAudioToBeGenerated(void*, ClownMDEmu*, size_t,
    void (*)(ClownMDEmu *clownmdemu, cc_s16l *sampleBuffer, size_t totalFrames)) {}

/// Callback function that handles CDDA audio playback (not supported by this frontend).
void cddaAudioToBeGenerated(void*, ClownMDEmu*, size_t,
    void (*)(ClownMDEmu *clownmdemu, cc_s16l *sampleBuffer, size_t totalFrames)) {}

/// Callback function that seeks to a given sector on the emulated CD (not supported by this frontend).
void cdSeeked(void*, cc_u32f) {}

/// Callback function that reads the current sector on the emulated CD (not supported by this frontend).
void cdSectorRead(void*, cc_u16l*) {}

/// Callback function that seeks to a given track on the emulated audio CD (not supported by this frontend).
cc_bool cdTrackSeeked(void*, cc_u16f, ClownMDEmu_CDDAMode) {
    return false;
}

/// Callback function that read the current track on the emulated audio CD (not supported by this frontend).
uint32_t cdAudioRead(void*, cc_s16l*, size_t) {
    return 0;
}

/// Callback function that opens the savegame file with read privileges.
cc_bool saveFileOpenedForReading(void*, const char *filename) {
    saveFile = Util::Io::File(filename);
    if (!saveFile.exists()) {
        saveFile.create(Util::Io::File::REGULAR);
    }

    saveFileInputStream = new Util::Io::FileInputStream(saveFile);
    return true;
}

/// Callback function that reads from the savegame file.
cc_s16f saveFileRead(void*) {
    return saveFileInputStream->read();
}

/// Callback function that opens the savegame file with write privileges.
cc_bool saveFileOpenedForWriting(void *, const char *filename) {
    saveFile = Util::Io::File(filename);
    if (!saveFile.exists()) {
        saveFile.create(Util::Io::File::REGULAR);
    }

    saveFileOutputStream = new Util::Io::FileOutputStream(saveFile);
    return true;
}

/// Callback function that writes to the savegame file.
void saveFileWritten(void*, const cc_u8f byte) {
    saveFileOutputStream->write(byte);
}

/// Callback function that closes the savegame file.
void saveFileClosed(void*) {
    delete saveFileInputStream;
    delete saveFileOutputStream;
}

/// Callback function that removes the savegame file.
cc_bool saveFileRemoved(void*, const char *filename) {
    const Util::Io::File file(filename);
    if (!file.exists()) {
        return false;
    }

    return file.remove();
}

/// Callback function that gets the size of the savegame file.
cc_bool saveFileSizeObtained(void*, const char *filename, size_t *size) {
    const Util::Io::File file(filename);
    if (!file.exists()) {
        return false;
    }

    *size = file.getLength();
    return true;
}

int32_t main(int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.addArgument("resolution", false, "r");
    argumentParser.setHelpText(HELP_TEXT);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "clownmdemu: No arguments provided!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    // Read the ROM file
    Util::Io::File romFile(arguments[0]);
    Util::Io::FileInputStream stream(romFile);
    auto *fileBuffer = new uint8_t[romFile.getLength()];
    stream.read(fileBuffer, 0, romFile.getLength());

    const auto romLength = romFile.getLength() / 2;
    auto *rom = new cc_u16l[romLength];
    for (size_t i = 0; i < romLength; i++) {
        rom[i] = fileBuffer[i * 2] << 8 | fileBuffer[i * 2 + 1];
    }

    delete[] fileBuffer;

    // Initialize the linear frame buffer
    auto lfbFile = Util::Io::File("/device/lfb");
    if (argumentParser.hasArgument("resolution")) {
        const auto resolutionString = argumentParser.getArgument("resolution");
        Util::Graphic::LinearFrameBuffer::setResolution(lfbFile, resolutionString);
    }

    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    Util::Graphic::LinearFrameBuffer framebuffer(lfbFile);
    lfb = &framebuffer;
    lfb->clear();

    auto scanlineCallback = scanlineRendered32;
    if (lfb->getColorDepth() == 24) {
        scanlineCallback = scanlineRendered24;
    } else if (lfb->getColorDepth() == 16) {
        scanlineCallback = scanlineRendered16;
    } else if (lfb->getColorDepth() != 32) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION, "clownmdemu: Unsupported color depth!");
    }

    // Initialize the callback object
    callbacks = {
        nullptr,
        colorUpdated,
        scanlineCallback,
        inputRequested,
        fmAudioToBeGenerated,
        psgAudioToBeGenerated,
        pcmAudioToBeGenerated,
        cddaAudioToBeGenerated,
        cdSeeked,
        cdSectorRead,
        cdTrackSeeked,
        cdAudioRead,
        saveFileOpenedForReading,
        saveFileRead,
        saveFileOpenedForWriting,
        saveFileWritten,
        saveFileClosed,
        saveFileRemoved,
        saveFileSizeObtained
    };

    // Initialize the emulator
    configuration.general.region = CLOWNMDEMU_REGION_OVERSEAS;
    configuration.general.tv_standard = CLOWNMDEMU_TV_STANDARD_NTSC;

    targetFrameRate = configuration.general.tv_standard == CLOWNMDEMU_TV_STANDARD_NTSC ? 60 : 50;
    targetFrameTime = Util::Time::Timestamp::ofSecondsFloat(1.0f / static_cast<float>(targetFrameRate));

    ClownMDEmu_SetLogCallback(log, nullptr);
    ClownMDEmu_Constant_Initialise();
    ClownMDEmu_Initialise(&emulator, &configuration, &callbacks);
    ClownMDEmu_SetCartridge(&emulator, rom, romLength);
    ClownMDEmu_HardReset(&emulator, true, false);

    // Initialize keyboard input
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
    Util::Io::DeLayout layout;
    Util::Io::KeyDecoder keyDecoder(layout);

    // Enter main loop
    while(true) {
        // Get time for frame time measurement
        auto startTime = Util::Time::Timestamp::getSystemTime();

        // Read and process all available key events
        auto c = Util::System::in.read();
        while (c != -1 && keyDecoder.parseScancode(c)) {
            const auto keyEvent = keyDecoder.getKeyEvent();
            bool pressed = keyEvent.isPressed();

            switch (keyEvent.getScancode()) {
                case Util::Io::KeyEvent::UP:
                    buttons[CLOWNMDEMU_BUTTON_UP] = pressed;
                    break;
                case Util::Io::KeyEvent::DOWN:
                    buttons[CLOWNMDEMU_BUTTON_DOWN] = pressed;
                    break;
                case Util::Io::KeyEvent::LEFT:
                    buttons[CLOWNMDEMU_BUTTON_LEFT] = pressed;
                    break;
                case Util::Io::KeyEvent::RIGHT:
                    buttons[CLOWNMDEMU_BUTTON_RIGHT] = pressed;
                    break;
                case Util::Io::KeyEvent::A:
                    buttons[CLOWNMDEMU_BUTTON_A] = pressed;
                    break;
                case Util::Io::KeyEvent::S:
                    buttons[CLOWNMDEMU_BUTTON_B] = pressed;
                    break;
                case Util::Io::KeyEvent::D:
                    buttons[CLOWNMDEMU_BUTTON_C] = pressed;
                    break;
                case Util::Io::KeyEvent::Y:
                    buttons[CLOWNMDEMU_BUTTON_X] = pressed;
                    break;
                case Util::Io::KeyEvent::X:
                    buttons[CLOWNMDEMU_BUTTON_Y] = pressed;
                    break;
                case Util::Io::KeyEvent::C:
                    buttons[CLOWNMDEMU_BUTTON_Z] = pressed;
                    break;
                case Util::Io::KeyEvent::SPACE:
                    buttons[CLOWNMDEMU_BUTTON_START] = pressed;
                    break;
                case Util::Io::KeyEvent::ENTER:
                    buttons[CLOWNMDEMU_BUTTON_MODE] = pressed;
                    break;
                case Util::Io::KeyEvent::F1:
                    if (scale > 1) {
                        scale--;
                        updateScreenVariables(mdScreenWidth, mdScreenHeight, true);
                    }
                    break;
                case Util::Io::KeyEvent::F2:
                    if (scale < maxScale) {
                        scale++;
                        updateScreenVariables(mdScreenWidth, mdScreenHeight, true);
                    }
                    break;
                case Util::Io::KeyEvent::ESC:
                    return 0;
                default:
                    break;
            }

            c = Util::System::in.read();
        }

        // Emulate the next frame -> Automatically calls scanline draw function to update the screen
        ClownMDEmu_Iterate(&emulator);

        // Update FPS display
        const auto fpsString = Util::String::format("FPS: %u", fps);
        lfb->drawString(Util::Graphic::Fonts::TERMINAL_8x8, 0, 0, static_cast<const char*>(fpsString),
            Util::Graphic::Colors::WHITE, Util::Graphic::Colors::BLACK);

        // Sleep to hit the target framerate
        auto renderTime = Util::Time::Timestamp::getSystemTime() - startTime;
        if (renderTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - renderTime);
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