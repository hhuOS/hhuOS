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

#include "WindowManager.h"

#include "util/graphic/Colors.h"
#include "util/graphic/font/Terminal8x8.h"
#include "util/io/stream/NumberUtil.h"
#include "util/base/Constants.h"
#include "kepler/Protocol.h"
#include "lib/interface.h"

WindowManager::WindowManager(Util::Graphic::LinearFrameBuffer &lfb) : lfb(lfb) {
    const auto idString = Util::String::format("%u", processId);

    auto desktopFile = Util::Io::File("/system/kepler");
    desktopFile.create(Util::Io::File::REGULAR);

    auto desktopFileStream = Util::Io::FileOutputStream(desktopFile);
    desktopFileStream.write(static_cast<const uint8_t*>(idString), 0, idString.length());

    createNextPipe();
}

void WindowManager::run() {
    while (true) {
        for (auto *client : clients) {
            auto &inputStream = client->getInputStream();
            if (inputStream.isReadyToRead()) {
                const auto command = static_cast<Kepler::Command>(Util::Io::NumberUtil::readUnsigned8BitValue(inputStream));
                switch (command) {
                    case Kepler::CREATE_WINDOW:
                        createWindow(*client);
                        break;
                    case Kepler::FLUSH:
                        flushWindow(*client);
                        break;
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
            clients.add(new Client(nextClientId, nextPipe, outputPipe));

            createNextPipe();
        }
    }
}


void WindowManager::createNextPipe() {
    nextClientId = clientIdGenerator.next();
    createPipe(Util::String::format("%u", nextClientId));
    nextPipe = new Util::Io::FileInputStream(Util::String::format("/process/%u/pipes/%u", processId, nextClientId));
    nextPipe->setAccessMode(Util::Io::File::NON_BLOCKING);
}

void WindowManager::createWindow(Client &client) {
    auto &inputStream = client.getInputStream();
    auto &outputStream = client.getOutputStream();

    inputStream.setAccessMode(Util::Io::File::BLOCKING);
    auto request = Kepler::Request::CreateWindow();
    request.readFromStream(inputStream);
    inputStream.setAccessMode(Util::Io::File::NON_BLOCKING);

    const auto windowId = windowIdGenerator.next();
    const auto bufferSize = 320 * 240 * lfb.getBytesPerPixel();
    const auto bufferPages = bufferSize % Util::PAGESIZE == 0 ? bufferSize / Util::PAGESIZE : bufferSize / Util::PAGESIZE + 1;
    auto *sharedBuffer = new Util::Async::SharedMemory(Util::String::format("%u", windowId), bufferPages);
    sharedBuffer->publish();

    const auto width = 320;
    const auto height = 240;
    const auto posX = windowId == 0 || windowId == 2 ? 10 : 400;
    const auto posY = windowId == 0 || windowId == 1 ? 10 : 300;

    client.addWindow(new ClientWindow(windowId, sharedBuffer, posX, posY, width, height, request.getTitle()));

    const auto response = Kepler::Response::CreateWindow(windowId, width, height, lfb.getColorDepth());
    response.writeToStream(outputStream);
}

void WindowManager::flushWindow(const Client &client) const {
    auto &inputStream = client.getInputStream();
    auto &outputStream = client.getOutputStream();

    inputStream.setAccessMode(Util::Io::File::BLOCKING);
    auto request = Kepler::Request::Flush();
    request.readFromStream(inputStream);
    inputStream.setAccessMode(Util::Io::File::NON_BLOCKING);

    auto *window = client.getWindowById(request.getWindowId());
    if (window == nullptr) {
        const auto response = Kepler::Response::Flush(false);
        response.writeToStream(outputStream);
        return;
    }

    const auto &font = Util::Graphic::Fonts::TERMINAL_8x8;
    const auto &title = window->getTitle();
    const auto &buffer = window->getBuffer();
    const auto posX = window->getPosX();
    const auto posY = window->getPosY();
    const auto width = window->getWidth();
    const auto height = window->getHeight();

    lfb.drawRectangle(posX, posY, width + 2, height + font.getCharHeight() + 5, Util::Graphic::Colors::WHITE);
    lfb.fillRectangle(posX, posY, width + 2, font.getCharHeight() + 5, Util::Graphic::Colors::WHITE);

    const auto titleWidth = static_cast<uint16_t>(title.length() * font.getCharWidth());
    const auto titlePosX = posX + (width + 2 - titleWidth) / 2;
    lfb.drawString(font, titlePosX, posY + 2, title, Util::Graphic::Colors::BLACK, Util::Graphic::Colors::WHITE);

    auto sourceAddress = buffer.getAddress();
    auto targetAddress = lfb.getBuffer().add((posY + font.getCharHeight() + 4) * lfb.getPitch() + (posX + 1) * lfb.getBytesPerPixel());

    for (uint16_t y = 0; y < height; y++) {
        targetAddress.copyRange(sourceAddress, width * lfb.getBytesPerPixel());
        targetAddress = targetAddress.add(lfb.getPitch());
        sourceAddress = sourceAddress.add(width * lfb.getBytesPerPixel());
    }

    const auto response = Kepler::Response::Flush(true);
    response.writeToStream(outputStream);
}
