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

#include "util/io/stream/NumberUtil.h"
#include "util/base/Constants.h"
#include "util/graphic/Colors.h"
#include "kepler/Protocol.h"
#include "util/io/key/MouseDecoder.h"
#include "lib/interface.h"

const Util::Time::Timestamp WindowManager::TARGET_FRAMETIME =
    Util::Time::Timestamp::ofNanoseconds(1000000000 / TARGET_FPS);

WindowManager::WindowManager(Util::Graphic::LinearFrameBuffer &lfb) : lfb(lfb), doubleLfb(lfb),
    tripleLfb(reinterpret_cast<Util::Graphic::LinearFrameBuffer&>(doubleLfb))
{
    const auto idString = Util::String::format("%u", processId);

    auto desktopFile = Util::Io::File("/system/kepler");
    desktopFile.create(Util::Io::File::REGULAR);

    auto desktopFileStream = Util::Io::FileOutputStream(desktopFile);
    desktopFileStream.write(static_cast<const uint8_t*>(idString), 0, idString.length());

    createNextPipe();
}

void WindowManager::run() {
    auto lastFlushTime = Util::Time::Timestamp::getSystemTime();
    auto lastTimestamp = Util::Time::Timestamp::getSystemTime();
    auto fpsTimer = Util::Time::Timestamp();
    size_t fpsCounter = 0;

    auto mouseInputStream = Util::Io::FileInputStream("/device/mouse");
    uint8_t mouseInputBuffer[4] = {0, 0, 0, 0};
    size_t mouseInputIndex = 0;

    mouseInputStream.setAccessMode(Util::Io::File::NON_BLOCKING);

    while (true) {
        bool yield = true; // Yield to another process if no work has been done in this iteration

        // Handle mouse input
        const auto mouseByte = mouseInputStream.read();
        if (mouseByte >= 0) {
            mouseInputBuffer[mouseInputIndex++] = static_cast<uint8_t>(mouseByte);
            if (mouseInputIndex == 4) {
                const auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseInputBuffer);

                if (mouseUpdate.xMovement != 0 || mouseUpdate.yMovement != 0) {
                    mouseX += mouseUpdate.xMovement;
                    mouseY -= mouseUpdate.yMovement;

                    if (mouseX < 0) {
                        mouseX = 0;
                    } else if (mouseX >= static_cast<int32_t>(lfb.getResolutionX())) {
                        mouseX = lfb.getResolutionX() - 1;
                    }

                    if (mouseY < 0) {
                        mouseY = 0;
                    } else if (mouseY >= static_cast<int32_t>(lfb.getResolutionY())) {
                        mouseY = lfb.getResolutionY() - 1;
                    }

                    needRedraw = true;
                }

                mouseInputIndex = 0;
            }

            yield = false; // Work has been done, do not yield
        }

        // Handle new client connections
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
            yield = false; // Work has been done, do not yield
        }

        // Handle client commands
        for (auto *client : clients) {
            auto &inputStream = client->getInputStream();
            if (inputStream.isReadyToRead()) {
                const auto command = static_cast<Kepler::Command>(
                    Util::Io::NumberUtil::readUnsigned8BitValue(inputStream));

                switch (command) {
                    case Kepler::CREATE_WINDOW:
                        createWindow(*client);
                        needRedraw = true;
                        break;
                    case Kepler::FLUSH:
                        flushWindow(*client);
                        needRedraw = true;
                        break;
                    default:
                        break;
                }

                yield = false; // Work has been done, do not yield
            }
        }

        // Draw dirty windows
        for (size_t i = 0; i < clients.size(); i++) {
            const auto &client = *clients.get(i);
            const auto &windows = client.getWindows();

            for (size_t j = 0; j < windows.size(); j++) {
                const auto &window = windows.get(j);

                if (window->isDirty()) {
                    window->drawFrame(tripleLfb, i == 0 && j == 0 ?
                        Util::Graphic::Colors::HHU_BLUE : Util::Graphic::Colors::HHU_ICE_BLUE);
                    window->flush(tripleLfb);
                    window->setDirty(false);
                    needRedraw = true;
                    yield = false; // Work has been done, do not yield
                }
            }
        }

        // Flush frame buffer at target framerate
        const auto now = Util::Time::Timestamp::getSystemTime();
        fpsTimer += now - lastTimestamp;
        lastTimestamp = now;

        if (now - lastFlushTime >= TARGET_FRAMETIME) {
            fpsCounter++;

            if (fpsTimer >= Util::Time::Timestamp::ofSeconds(1)) {
                tripleLfb.drawString(Util::Graphic::Fonts::TERMINAL_8x8, 0, 0,
                    static_cast<const char*>(Util::String::format("FPS: %u", fpsCounter)),
                    Util::Graphic::Colors::WHITE, Util::Graphic::Colors::BLACK);

                needRedraw = true;
                fpsTimer = Util::Time::Timestamp();
                fpsCounter = 0;
            }

            if (needRedraw) {
                // Flush windows into double buffer
                tripleLfb.flush();

                // Draw mouse cursor into double buffer (on top of windows)
                doubleLfb.drawLine(mouseX - 10, mouseY, mouseX + 10, mouseY, Util::Graphic::Colors::RED);
                doubleLfb.drawLine(mouseX, mouseY - 10, mouseX, mouseY + 10, Util::Graphic::Colors::RED);

                // Flush double buffer to screen
                doubleLfb.flush();
                needRedraw = false;
            }

            lastFlushTime = now;
            yield = false; // Work has been done, do not yield
        }

        // Yield if no work has been done (i.e. no input, no commands, no redraw)
        if (yield) {
            Util::Async::Thread::yield();
        }
    }
}


void WindowManager::createNextPipe() {
    nextClientId = clientIdGenerator.getNextId();
    createPipe(Util::String::format("%u", nextClientId));
    nextPipe = new Util::Io::FileInputStream(
        Util::String::format("/process/%u/pipes/%u", processId, nextClientId));
    nextPipe->setAccessMode(Util::Io::File::NON_BLOCKING);
}

void WindowManager::createWindow(Client &client) {
    auto &inputStream = client.getInputStream();
    auto &outputStream = client.getOutputStream();

    inputStream.setAccessMode(Util::Io::File::BLOCKING);
    auto request = Kepler::Request::CreateWindow();
    request.readFromStream(inputStream);
    inputStream.setAccessMode(Util::Io::File::NON_BLOCKING);

    const auto windowId = windowIdGenerator.getNextId();
    const auto bufferSize = 320 * 240 * tripleLfb.getBytesPerPixel();
    const auto bufferPages = bufferSize % Util::PAGESIZE == 0 ?
        bufferSize / Util::PAGESIZE : bufferSize / Util::PAGESIZE + 1;
    auto *sharedBuffer = new Util::Async::SharedMemory(Util::String::format("%u", windowId), bufferPages);
    sharedBuffer->publish();

    constexpr auto width = 320;
    constexpr auto height = 240;
    const auto posX = windowId == 0 || windowId == 2 ? 32 : 400;
    const auto posY = windowId == 0 || windowId == 1 ? 32 : 320;

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

    const auto *window = client.getWindowById(request.getWindowId());
    if (window == nullptr) {
        const auto response = Kepler::Response::Flush(false);
        response.writeToStream(outputStream);
        return;
    }

    window->flush(tripleLfb);

    const auto response = Kepler::Response::Flush(true);
    response.writeToStream(outputStream);
}
