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

#include "lib/interface.h"
#include "util/graphic/Colors.h"
#include "util/async/IdGenerator.h"
#include "util/async/SharedMemory.h"
#include "util/base/Constants.h"
#include "util/collection/HashMap.h"
#include "util/graphic/Ansi.h"
#include "util/graphic/LinearFrameBuffer.h"
#include "util/graphic/font/Terminal8x8.h"
#include "util/io/stream/FileInputStream.h"
#include "util/io/stream/FileOutputStream.h"
#include "util/io/stream/NumberUtil.h"
#include "kepler/Protocol.h"

struct Window {
    Util::Io::FileInputStream *inputStream;
    Util::Io::FileOutputStream *outputStream;
    Util::Async::SharedMemory *buffer;
    uint16_t posX;
    uint16_t posY;
    uint16_t resX;
    uint16_t resY;
    Util::String title;
};

auto idGenerator = Util::Async::IdGenerator();
auto windows = Util::HashMap<size_t, Window>();

void main() {
    Util::Graphic::Ansi::prepareGraphicalApplication(true);

    auto lfbFile = Util::Io::File("/device/lfb");
    auto lfb = Util::Graphic::LinearFrameBuffer::open(lfbFile);
    lfb.clear();

    const auto processId = Util::Async::Process::getCurrentProcess().getId();
    auto nextId = idGenerator.next();
    createPipe(Util::String::format("%u", nextId));
    auto *nextPipe = new Util::Io::FileInputStream(Util::String::format("/process/%u/pipes/%u", processId, nextId));
    nextPipe->setAccessMode(Util::Io::File::NON_BLOCKING);

    auto desktopFile = Util::Io::File("/system/kepler");
    desktopFile.create(Util::Io::File::REGULAR);
    auto desktopFileStream = Util::Io::FileOutputStream(desktopFile);
    const auto idString = Util::String::format("%u", Util::Async::Process::getCurrentProcess().getId());
    desktopFileStream.write(static_cast<const uint8_t*>(idString), 0, idString.length());

    while (true) {
        for (auto &id : windows.getKeys()) {
            auto window = windows.get(id);
            if (window.inputStream->isReadyToRead()) {
                const auto command = static_cast<Kepler::Command>(Util::Io::NumberUtil::readUnsigned8BitValue(*window.inputStream));
                switch (command) {
                    case Kepler::CREATE_WINDOW: {
                        nextPipe->setAccessMode(Util::Io::File::BLOCKING);
                        auto request = Kepler::Request::CreateWindow();
                        request.readFromStream(*window.inputStream);
                        nextPipe->setAccessMode(Util::Io::File::NON_BLOCKING);

                        auto bufferSize = 320 * 240 * lfb.getBytesPerPixel();
                        auto bufferPages = bufferSize % Util::PAGESIZE == 0 ? bufferSize / Util::PAGESIZE : bufferSize / Util::PAGESIZE + 1;
                        auto *sharedBuffer = new Util::Async::SharedMemory(Util::String::format("%u", nextId), bufferPages);
                        sharedBuffer->publish();

                        window.buffer = sharedBuffer;
                        window.posX = id == 0 || id == 2 ? 10 : 400;
                        window.posY = id == 0 || id == 1 ? 10 : 300;
                        window.resX = 320;
                        window.resY = 240;
                        window.title = request.getTitle();

                        windows.put(id, window);

                        const auto response = Kepler::Response::CreateWindow{window.resX, window.resY, lfb.getColorDepth(), nextId};
                        response.writeToStream(*window.outputStream);

                        break;
                    }
                    case Kepler::FLUSH: {
                        const auto &font = Util::Graphic::Fonts::TERMINAL_8x8;

                        lfb.drawRectangle(window.posX, window.posY, window.resX + 2, window.resY + font.getCharHeight() + 5, Util::Graphic::Colors::WHITE);
                        lfb.drawLine(window.posX, window.posY + font.getCharHeight() + 3, window.posX + window.resX + 1, window.posY + font.getCharHeight() + 3, Util::Graphic::Colors::WHITE);

                        const auto titleWidth = static_cast<uint16_t>(window.title.length() * font.getCharWidth());
                        const auto titlePosX = window.posX + (window.resX + 2 - titleWidth) / 2;
                        lfb.drawString(font, titlePosX, window.posY + 2, window.title, Util::Graphic::Colors::BLACK, Util::Graphic::Colors::WHITE);

                        auto sourceAddress = window.buffer->getAddress();
                        auto targetAddress = lfb.getBuffer().add((window.posY + font.getCharHeight() + 4) * lfb.getPitch() + (window.posX + 1) * lfb.getBytesPerPixel());

                        for (uint16_t y = 0; y < window.resY; y++) {
                            targetAddress.copyRange(sourceAddress, window.resX * lfb.getBytesPerPixel());
                            targetAddress = targetAddress.add(lfb.getPitch());
                            sourceAddress = sourceAddress.add(window.resX * lfb.getBytesPerPixel());
                        }

                        break;
                    }
                    default:
                        break;
                }
            }
        }

        if (nextPipe->isReadyToRead()) {
            nextPipe->setAccessMode(Util::Io::File::BLOCKING);
            auto command = static_cast<Kepler::Command>(Util::Io::NumberUtil::readUnsigned8BitValue(*nextPipe));
            if (command != Kepler::CONNECT) {
                continue;
            }

            auto request = Kepler::Request::Connect();
            request.readFromStream(*nextPipe);
            nextPipe->setAccessMode(Util::Io::File::NON_BLOCKING);

            auto *outputPipe = new Util::Io::FileOutputStream(request.getPipePath());
            windows.put(nextId, Window{nextPipe, outputPipe, nullptr, 0, 0, 0, 0, ""});

            nextId = idGenerator.next();
            createPipe(Util::String::format("%u", nextId));
            nextPipe = new Util::Io::FileInputStream(Util::String::format("/process/%u/pipes/%u", processId, nextId));
            nextPipe->setAccessMode(Util::Io::File::NON_BLOCKING);
        }
    }
}
