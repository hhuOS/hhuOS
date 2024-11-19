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
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/graphic/LineDrawer.h"
#include "lib/util/collection/Array.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/InputStream.h"

static const constexpr uint16_t DEFAULT_FPS = 15;

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Play asciimation movies from text-files.\n"
                               "See https://http://www.asciimation.co.nz for more information\n"
                               "Usage: asciimate [FILE]\n"
                               "Options:\n"
                               "  -f, --framesPerSecond: Set the target framerate (Default: 15)\n"
                               "  -r, --resolution: Set display resolution\n"
                               "  -s, --scale: Set display scale factor (Must be <= 1; The application will be rendered at a lower internal resolution and scaled up/centered to fill the screen)\n"
                               "  -h, --help: Show this help message");

    argumentParser.addArgument("framesPerSecond", false, "f");
    argumentParser.addArgument("resolution", false, "r");
    argumentParser.addArgument("scale", false, "s");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "asciimate: No arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto file = Util::Io::File(arguments[0]);
    if (!file.exists() || file.isDirectory()) {
        Util::System::error << "asciimate: '" << arguments[0] << "' could not be opened!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto lfbFile = Util::Io::File("/device/lfb");

    if (argumentParser.hasArgument("resolution")) {
        auto split1 = argumentParser.getArgument("resolution").split("x");
        auto split2 = split1[1].split("@");

        uint32_t resolutionX = Util::String::parseInt(split1[0]);
        uint32_t resolutionY = Util::String::parseInt(split2[0]);
        uint32_t colorDepth = split2.length() > 1 ? Util::String::parseInt(split2[1]) : 32;

        lfbFile.controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
    }

    auto scaleFactor = argumentParser.hasArgument("scale") ? Util::String::parseDouble(argumentParser.getArgument("scale")) : 1.0;
    auto lfb = Util::Graphic::LinearFrameBuffer(lfbFile);
    auto bufferedLfb = Util::Graphic::BufferedLinearFrameBuffer(lfb, scaleFactor);
    auto pixelDrawer = Util::Graphic::PixelDrawer(bufferedLfb);
    auto lineDrawer = Util::Graphic::LineDrawer(pixelDrawer);
    auto stringDrawer = Util::Graphic::StringDrawer(pixelDrawer);

    auto inputStream = Util::Io::FileInputStream(file);
    auto bufferedStream = Util::Io::BufferedInputStream(inputStream);
    bool endOfFile = false;

    auto frameInfo = bufferedStream.readLine(endOfFile).split(",");

    const auto &font = Util::Graphic::Font::getFontForResolution(bufferedLfb.getResolutionY());
    auto charWidth = font.getCharWidth();
    auto charHeight = font.getCharHeight();

    double fps = argumentParser.hasArgument("framesPerSecond") ? Util::String::parseInt(argumentParser.getArgument("framesPerSecond")) : DEFAULT_FPS;
    uint16_t rows = Util::String::parseInt(frameInfo[0]);
    uint16_t columns = Util::String::parseInt(frameInfo[1]);
    uint16_t frameStartX = (((bufferedLfb.getResolutionX() / charWidth) - columns) / 2) * charWidth;
    uint16_t frameStartY = (((bufferedLfb.getResolutionY() / charHeight) - rows) / 2) * charHeight;
    uint16_t frameEndX = frameStartX + (columns * charWidth);
    uint16_t frameEndY = frameStartY + (rows * charHeight);

    Util::Graphic::Ansi::prepareGraphicalApplication(false);
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    while (true) {
        if (Util::System::in.read() > 0) {
            break;
        }

        auto delayLine = bufferedStream.readLine(endOfFile);
        if (delayLine.length() == 0) {
            break;
        }

        uint32_t delay = Util::String::parseInt(delayLine);
        bufferedLfb.clear();

        lineDrawer.drawLine(frameStartX - charWidth, frameStartY - charHeight, frameEndX + charWidth, frameStartY - charHeight, Util::Graphic::Colors::WHITE);
        lineDrawer.drawLine(frameStartX - charWidth, frameEndY + charHeight, frameEndX + charWidth, frameEndY + charHeight, Util::Graphic::Colors::WHITE);
        lineDrawer.drawLine(frameStartX - charWidth, frameStartY - charHeight, frameStartX - charWidth, frameEndY + charHeight, Util::Graphic::Colors::WHITE);
        lineDrawer.drawLine(frameEndX + charWidth, frameStartY - charHeight, frameEndX + charWidth, frameEndY + charHeight, Util::Graphic::Colors::WHITE);

        for (int16_t i = 0; i < rows - 1; i++) {
            stringDrawer.drawString(font, frameStartX, frameStartY + charHeight * i, static_cast<const char*>(bufferedStream.readLine(endOfFile)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::BLACK);
        }

        bufferedLfb.flush();
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(static_cast<uint32_t>(delay * (1000 / fps))));
    }

    Util::Graphic::Ansi::cleanupGraphicalApplication();
    return 0;
}