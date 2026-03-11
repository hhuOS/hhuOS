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

#include <util/base/ArgumentParser.h>
#include <util/base/String.h>
#include <util/base/System.h>
#include <util/collection/Array.h>
#include <util/graphic/Ansi.h>
#include <util/graphic/LinearFrameBuffer.h>
#include <util/graphic/BufferedLinearFrameBuffer.h>
#include <util/graphic/Colors.h>
#include <util/graphic/Font.h>
#include <util/graphic/font/Mini4x6.h>
#include <util/graphic/font/Terminal8x16.h>
#include <util/graphic/font/Terminal8x8.h>
#include <util/io/file/File.h>
#include <util/io/stream/InputStream.h>
#include <util/io/stream/FileInputStream.h>
#include <util/io/stream/BufferedInputStream.h>
#include <util/io/stream/PrintStream.h>
#include <util/io/key/KeyDecoder.h>
#include <util/io/key/layout/DeLayout.h>
#include <util/async/Thread.h>
#include <util/time/Timestamp.h>

const char *HELP_TEXT =
#include "generated/README.md"
;

/// Selects a font based on the vertical resolution of the framebuffer.
const Util::Graphic::Font& selectFont(const uint16_t resolutionY) {
    if (resolutionY < 350) {
        return Util::Graphic::Fonts::MINI_4x6;
    }

    if (resolutionY < 500) {
        return Util::Graphic::Fonts::TERMINAL_8x8;
    }

    return Util::Graphic::Fonts::TERMINAL_8x16;
}

int32_t main(int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_TEXT);

    argumentParser.addArgument("fps", false, "f");
    argumentParser.addArgument("resolution", false, "r");
    argumentParser.addArgument("scale", false, "s");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "asciimate: No arguments provided!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto file = Util::Io::File(arguments[0]);
    if (!file.exists() || file.isDirectory()) {
        Util::System::error << "asciimate: Failed to open '" << arguments[0] << "'!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto lfbFile = Util::Io::File("/device/lfb");
    if (argumentParser.hasArgument("resolution")) {
        const auto resolution = argumentParser.getArgument("resolution");
        Util::Graphic::LinearFrameBuffer::setResolution(lfbFile, resolution);
    }

    const auto scaleString = argumentParser.getArgument("scale", "1.0");
    const auto scaleFactor = Util::String::parseFloat<float>(scaleString);
    const Util::Graphic::LinearFrameBuffer lfb(lfbFile);
    const Util::Graphic::BufferedLinearFrameBuffer bufferedLfb(lfb, scaleFactor);

    Util::Io::FileInputStream fileInputStream(file);
    Util::Io::BufferedInputStream inputStream(fileInputStream);

    const auto &font = selectFont(bufferedLfb.getResolutionY());
    const auto charWidth = font.getCharWidth();
    const auto charHeight = font.getCharHeight();

    const auto fpsString = argumentParser.getArgument("fps", "15");
    const auto fps = Util::String::parseNumber<size_t>(fpsString);
    const auto timePerFrame = Util::Time::Timestamp::ofMicroseconds(1000000 / fps);

    const auto frameInfo = inputStream.readLine().content.split(",");
    const auto rows = Util::String::parseNumber<uint16_t>(frameInfo[0]);
    const auto columns = Util::String::parseNumber<uint16_t>(frameInfo[1]);
    const auto frameStartX = (bufferedLfb.getResolutionX() / charWidth - columns) / 2 * charWidth;
    const auto frameStartY = (bufferedLfb.getResolutionY() / charHeight - rows) / 2 * charHeight;
    const auto frameEndX = frameStartX + columns * charWidth;
    const auto frameEndY = frameStartY + rows * charHeight;

    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    const Util::Io::DeLayout layout;
    Util::Io::KeyDecoder keyDecoder(layout);

    while (true) {
        // Exit application if ESC is pressed
        if (Util::System::in.isReadyToRead()) {
            if (keyDecoder.parseScancode(Util::System::in.read())) {
                if (keyDecoder.getKeyEvent().getScancode() == Util::Io::KeyEvent::ESC) {
                    break;
                }
            }
        }

        // Each frame starts with a line containing the delay.
        // The delay is given in frames (e.g. a delay of 2 means that the frame should be displayed for 2 frames,
        // which equals 2 * timePerFrame).
        const auto delayLine = inputStream.readLine();
        if (delayLine.content.isEmpty()) {
            // End of file reached, or invalid format
            break;
        }

        const auto delay = Util::String::parseNumber<size_t>(delayLine.content);

        bufferedLfb.clear();

        // Draw a border around the frame
        bufferedLfb.drawLine(frameStartX - charWidth, frameStartY - charHeight,
            frameEndX + charWidth, frameStartY - charHeight, Util::Graphic::Colors::WHITE);
        bufferedLfb.drawLine(frameStartX - charWidth, frameEndY + charHeight,
            frameEndX + charWidth, frameEndY + charHeight, Util::Graphic::Colors::WHITE);
        bufferedLfb.drawLine(frameStartX - charWidth, frameStartY - charHeight,
            frameStartX - charWidth, frameEndY + charHeight, Util::Graphic::Colors::WHITE);
        bufferedLfb.drawLine(frameEndX + charWidth, frameStartY - charHeight,
            frameEndX + charWidth, frameEndY + charHeight, Util::Graphic::Colors::WHITE);

        // Read the frame line by line and draw it to the framebuffer
        for (size_t i = 0; i < rows; i++) {
            const auto line = inputStream.readLine();
            bufferedLfb.drawString(font, frameStartX, frameStartY + charHeight * i, line.content,
                Util::Graphic::Colors::WHITE, Util::Graphic::Colors::BLACK);
        }

        // Flush the framebuffer and wait for the specified delay before drawing the next frame
        bufferedLfb.flush();
        Util::Async::Thread::sleep(timePerFrame * delay);
    }

    Util::Graphic::Ansi::cleanupGraphicalApplication();
    return 0;
}