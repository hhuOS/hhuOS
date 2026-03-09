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

#include "WindowManager.h"

#include "util/io/stream/NumberUtil.h"
#include "util/base/Constants.h"
#include "util/graphic/Colors.h"
#include "kepler/Protocol.h"
#include "util/io/key/MouseDecoder.h"
#include "lib/interface.h"
#include "util/base/System.h"

const Util::Time::Timestamp WindowManager::TARGET_FRAMETIME =
    Util::Time::Timestamp::ofNanoseconds(1000000000 / TARGET_FPS);

WindowManager::WindowManager(Util::Graphic::LinearFrameBuffer &lfb) : lfb(lfb), doubleLfb(lfb),
    tripleLfb(reinterpret_cast<Util::Graphic::LinearFrameBuffer&>(doubleLfb)), mouseInputHandler(lfb.getResolutionX(), lfb.getResolutionY())
{
    const auto idString = Util::String::format("%u", processId);

    const auto desktopFile = Util::Io::File("/system/kepler");
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

    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    while (true) {
        bool yield = true; // Yield to another process if no work has been done in this iteration

        // Handle new client connections
        if (nextPipe->isReadyToRead()) {
            nextPipe->setAccessMode(Util::Io::File::BLOCKING);
            const auto command = static_cast<Kepler::Command>(
                Util::Io::NumberUtil::readUnsigned8BitValue(*nextPipe));

            if (command != Kepler::CONNECT) {
                continue;
            }

            auto request = Kepler::Request::Connect();
            request.readFromStream(*nextPipe);
            nextPipe->setAccessMode(Util::Io::File::NON_BLOCKING);

            auto *outputPipe = new Util::Io::FileOutputStream(request.getPipePath());
            clients.add(new Client(nextClientId, request.getProcessId(), nextPipe, outputPipe));

            createNextPipe();
            yield = false; // Work has been done, do not yield
        }

        // Handle client commands
        for (const auto *client : clients) {
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

        // Check windows against mouse positions
        if (mouseInputHandler.checkMouseInput()) {
            dispatchMouseEvents();
            yield = false; // Work has been done, do not yield
        }

        // Check keyboard input for focussed window
        const auto keyboardInput = Util::System::in.read();
        if (keyboardInput >= 0) {
            if (keyDecoder.parseScancode(keyboardInput)) {
                const auto key = keyDecoder.getKeyEvent();
                windowStack.getFocussedWindow()->sendKeyEvent(Kepler::Event::KeyEvent(key));
            }
        }

        // Draw dirty windows
        for (auto *window : windowStack) {
            if (window->isDirty()) {
                window->drawFrame(tripleLfb, windowStack.isFocussed(window) ?
                    Util::Graphic::Colors::HHU_BLUE : Util::Graphic::Colors::HHU_ICE_BLUE);
                window->flush(tripleLfb);
                window->setDirty(false);
                needRedraw = true;
                yield = false; // Work has been done, do not yield
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
                const auto mouseX = mouseInputHandler.getMousePosX();
                const auto mouseY = mouseInputHandler.getMousePosY();
                const auto xStart = mouseX - 10 < 0 ? 0 : mouseX - 10;
                const auto yStart = mouseY - 10 < 0 ? 0 : mouseY - 10;
                doubleLfb.drawLine(xStart, mouseY, mouseX + 10, mouseY, Util::Graphic::Colors::RED);
                doubleLfb.drawLine(mouseX, yStart, mouseX, mouseY + 10, Util::Graphic::Colors::RED);

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

void WindowManager::dispatchMouseEvents() {
    const auto mouseX = mouseInputHandler.getMousePosX();
    const auto mouseY = mouseInputHandler.getMousePosY();

    auto *mouseHoveredWindow = windowStack.getWindowAt(mouseX, mouseY);
    if (mouseHoveredWindow != nullptr) {
        const auto windowMouseCoords = mouseHoveredWindow->containsPoint(mouseX, mouseY);

        // Check if mouse is inside  the window, but outside its content area (e.g. title bar or borders).
        // In this case, we want to set the focus to the window on mouse click, but not send a mouse event.
        if (windowMouseCoords.y < 0) {
            if (mouseInputHandler.wasButtonPressed(Util::Io::MouseDecoder::LEFT_BUTTON)
                || mouseInputHandler.wasButtonPressed(Util::Io::MouseDecoder::RIGHT_BUTTON)
                || mouseInputHandler.wasButtonPressed(Util::Io::MouseDecoder::MIDDLE_BUTTON))
            {
                windowStack.setFocus(mouseHoveredWindow);
            }

            // If the left mouse button is pressed inside the title bar, start dragging the window
            if (mouseInputHandler.isButtonCurrentlyPressed(Util::Io::MouseDecoder::LEFT_BUTTON) && windowMouseCoords.y < 0) {
                if (isDragging) {
                    const auto diffX = mouseX - lastDragX;
                    const auto diffY = mouseY - lastDragY;
                    mouseHoveredWindow->setPosX(mouseHoveredWindow->getPosX() + diffX);
                    mouseHoveredWindow->setPosY(mouseHoveredWindow->getPosY() + diffY);
                    mouseHoveredWindow->setDirty(true);
                }

                isDragging = true;
                lastDragX = mouseX;
                lastDragY = mouseY;
            } else if (mouseInputHandler.wasButtonReleased(Util::Io::MouseDecoder::LEFT_BUTTON)) {
                isDragging = false;
            }
        } else { // Mouse is inside the content area of the window, send mouse events
            if (mouseInputHandler.hasMousePositionChanged()) {
                mouseHoveredWindow->sendMouseHoverEvent(Kepler::Event::MouseHover(windowMouseCoords.x, windowMouseCoords.y));
            }

            if (mouseInputHandler.wasButtonPressed(Util::Io::MouseDecoder::LEFT_BUTTON)) {
                // Left mouse button pressed
                windowStack.setFocus(mouseHoveredWindow);
                mouseHoveredWindow->sendMouseClickEvent(Kepler::Event::MouseClick(windowMouseCoords.x, windowMouseCoords.y,
                    Kepler::Event::MouseClick::LEFT, Kepler::Event::MouseClick::PRESS));
            } else if (mouseInputHandler.wasButtonReleased(Util::Io::MouseDecoder::LEFT_BUTTON)) {
                // Left mouse button released
                mouseHoveredWindow->sendMouseClickEvent(Kepler::Event::MouseClick(windowMouseCoords.x, windowMouseCoords.y,
                    Kepler::Event::MouseClick::LEFT, Kepler::Event::MouseClick::RELEASE));
            }

            if (mouseInputHandler.wasButtonPressed(Util::Io::MouseDecoder::RIGHT_BUTTON)) {
                // Right mouse button pressed
                windowStack.setFocus(mouseHoveredWindow);
                mouseHoveredWindow->sendMouseClickEvent(Kepler::Event::MouseClick(windowMouseCoords.x, windowMouseCoords.y,
                    Kepler::Event::MouseClick::RIGHT, Kepler::Event::MouseClick::PRESS));
            } else if (mouseInputHandler.wasButtonReleased(Util::Io::MouseDecoder::RIGHT_BUTTON)) {
                // Right mouse button released
                mouseHoveredWindow->sendMouseClickEvent(Kepler::Event::MouseClick(windowMouseCoords.x, windowMouseCoords.y,
                    Kepler::Event::MouseClick::RIGHT, Kepler::Event::MouseClick::RELEASE));
            }

            if (mouseInputHandler.wasButtonPressed(Util::Io::MouseDecoder::MIDDLE_BUTTON)) {
                // Middle mouse button pressed
                windowStack.setFocus(mouseHoveredWindow);
                mouseHoveredWindow->sendMouseClickEvent(Kepler::Event::MouseClick(windowMouseCoords.x, windowMouseCoords.y,
                    Kepler::Event::MouseClick::MIDDLE, Kepler::Event::MouseClick::PRESS));
            } else if (mouseInputHandler.wasButtonReleased(Util::Io::MouseDecoder::MIDDLE_BUTTON)) {
                // Middle mouse button released
                mouseHoveredWindow->sendMouseClickEvent(Kepler::Event::MouseClick(windowMouseCoords.x, windowMouseCoords.y,
                    Kepler::Event::MouseClick::MIDDLE, Kepler::Event::MouseClick::RELEASE));
            }
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

void WindowManager::createWindow(const Client &client) {
    auto &inputStream = client.getInputStream();
    auto &outputStream = client.getOutputStream();

    inputStream.setAccessMode(Util::Io::File::BLOCKING);
    auto request = Kepler::Request::CreateWindow();
    request.readFromStream(inputStream);

    const auto windowId = windowIdGenerator.getNextId();
    const auto bufferSize = 320 * 240 * ((tripleLfb.getColorDepth() + 7) / 8);
    const auto bufferPages = bufferSize % Util::PAGESIZE == 0 ?
        bufferSize / Util::PAGESIZE : bufferSize / Util::PAGESIZE + 1;
    auto *sharedBuffer = new Util::Async::SharedMemory(Util::String::format("%u", windowId), bufferPages);
    sharedBuffer->publish();

    constexpr auto width = 320;
    constexpr auto height = 240;
    const auto posX = windowId == 0 || windowId == 2 ? 32 : 400;
    const auto posY = windowId == 0 || windowId == 1 ? 32 : 320;

    const auto response = Kepler::Response::CreateWindow(windowId, width, height, lfb.getColorDepth());
    response.writeToStream(outputStream);

    const auto signal = static_cast<Kepler::Signal>(inputStream.read());
    inputStream.setAccessMode(Util::Io::File::NON_BLOCKING);

    if (signal == Kepler::CLIENT_WINDOW_INITIALIZED) {
        auto *window = new ClientWindow(windowId, client.getProcessId(), posX, posY, width, height, request.getTitle(), sharedBuffer);
        windowStack.push(window);
    }
}

void WindowManager::flushWindow(const Client &client) const {
    auto &inputStream = client.getInputStream();
    auto &outputStream = client.getOutputStream();

    inputStream.setAccessMode(Util::Io::File::BLOCKING);
    auto request = Kepler::Request::Flush();
    request.readFromStream(inputStream);
    inputStream.setAccessMode(Util::Io::File::NON_BLOCKING);

    const auto *window = windowStack.getWindowById(request.getWindowId());
    if (window == nullptr) {
        const auto response = Kepler::Response::Flush(false);
        response.writeToStream(outputStream);
        return;
    }

    window->flush(tripleLfb);

    const auto response = Kepler::Response::Flush(true);
    response.writeToStream(outputStream);
}
