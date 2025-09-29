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

#include "Protocol.h"
#include "async/IdGenerator.h"
#include "async/SharedMemory.h"
#include "base/Constants.h"
#include "collection/HashMap.h"
#include "collection/Pair.h"
#include "graphic/Ansi.h"
#include "graphic/LinearFrameBuffer.h"
#include "io/stream/FileInputStream.h"
#include "io/stream/FileOutputStream.h"
#include "io/stream/NumberUtil.h"
#include "lib/interface.h"

struct Window {
    Util::Io::FileInputStream *inputStream;
    Util::Io::FileOutputStream *outputStream;
    Util::Async::SharedMemory *buffer;
    uint16_t posX;
    uint16_t posY;
    uint16_t resX;
    uint16_t resY;
};

auto idGenerator = Util::Async::IdGenerator();
auto windows = Util::HashMap<size_t, Window>();

void main() {
    Util::Graphic::Ansi::prepareGraphicalApplication(true);

    auto lfbFile = Util::Io::File("/device/lfb");
    auto lfb = Util::Graphic::LinearFrameBuffer::open(lfbFile);
    lfb.clear();

    auto desktopFile = Util::Io::File("/system/desktop");
    desktopFile.create(Util::Io::File::REGULAR);

    auto desktopFileStream = Util::Io::FileOutputStream(desktopFile);
    const auto processId = Util::Async::Process::getCurrentProcess().getId();
    const auto idString = Util::String::format("%u", Util::Async::Process::getCurrentProcess().getId());
    desktopFileStream.write(static_cast<const uint8_t*>(idString), 0, idString.length());

    auto nextId = idGenerator.next();
    createPipe(Util::String::format("%u", nextId));
    auto *nextPipe = new Util::Io::FileInputStream(Util::String::format("/process/%u/pipes/%u", processId, nextId));
    nextPipe->setAccessMode(Util::Io::File::NON_BLOCKING);

    while (true) {
        for (auto &id : windows.getKeys()) {
            const auto &window = windows.get(id);
            if (window.inputStream->isReadyToRead()) {
                const auto command = static_cast<Command>(Util::Io::NumberUtil::readUnsigned8BitValue(*window.inputStream));
                switch (command) {
                    case FLUSH: {
                        auto sourceAddress = window.buffer->getAddress();
                        auto targetAddress = lfb.getBuffer().add(window.posY * lfb.getPitch() + window.posX * lfb.getBytesPerPixel());

                        for (uint16_t y = 0; y < window.resY; y++) {
                            targetAddress.copyRange(sourceAddress, window.resX * lfb.getBytesPerPixel());
                            targetAddress = targetAddress.add(lfb.getPitch());
                            sourceAddress = sourceAddress.add(window.resX * lfb.getBytesPerPixel());
                        }
                    }
                }
            }
        }

        if (nextPipe->isReadyToRead()) {
            nextPipe->setAccessMode(Util::Io::File::BLOCKING);
            const auto request = CreateWindowRequest::readFromStream(*nextPipe);
            nextPipe->setAccessMode(Util::Io::File::NON_BLOCKING);

            auto bufferSize = request.resX * request.resY * lfb.getBytesPerPixel();
            auto bufferPages = bufferSize % Util::PAGESIZE == 0 ? bufferSize / Util::PAGESIZE : bufferSize / Util::PAGESIZE + 1;
            auto *sharedBuffer = new Util::Async::SharedMemory(Util::String::format("%u", nextId), bufferPages);
            sharedBuffer->publish();

            uint16_t posX = nextId == 0 || nextId == 2 ? 0 : 400;
            uint16_t posY = nextId == 0 || nextId == 1 ? 0 : 300;

            auto *outputPipe = new Util::Io::FileOutputStream(request.getPipePath());
            windows.put(nextId, Window{nextPipe, outputPipe, sharedBuffer, posX, posY, request.resX, request.resY});

            const auto response = CreateWindowResponse{request.resX, request.resY, lfb.getColorDepth(), nextId, bufferPages};
            response.writeToStream(*outputPipe);

            nextId = idGenerator.next();
            createPipe(Util::String::format("%u", nextId));
            nextPipe = new Util::Io::FileInputStream(Util::String::format("/process/%u/pipes/%u", processId, nextId));
            nextPipe->setAccessMode(Util::Io::File::NON_BLOCKING);
        }
    }
}
