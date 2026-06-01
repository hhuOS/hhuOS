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

#include "Window.h"

#include "interface.h"
#include "kepler/Protocol.h"
#include "util/base/Constants.h"

namespace Kepler {

Window::Window(const uint16_t width, const uint16_t height, const Util::String &title, Client &client) : client(client) {
    if (!client.sendRequest(Request::CreateWindow(width, height, title))) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Window: Pipe closed!");
    }

    auto response = Response::CreateWindow();
    if (!client.receiveResponse(response)) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Window: Pipe closed!");
    }

    id = response.getId();

    const auto bytesPerPixel = (response.getColorDepth() == 15 ? 16 : response.getColorDepth()) / 8;
    const auto bufferSize = response.getWidth() * response.getHeight() * bytesPerPixel;
    const auto bufferPages = bufferSize % Util::PAGESIZE == 0 ?
        bufferSize / Util::PAGESIZE : bufferSize / Util::PAGESIZE + 1;

    sharedMemory = new Util::Async::SharedMemory(client.getWindowManagerProcessId(),
        Util::String::format("%u", id), bufferPages);

    if (!sharedMemory->map()) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Window: Failed to map frame buffer!");
    }

    auto *windowBuffer = reinterpret_cast<void*>(sharedMemory->getAddress().get());
    lfb = new Util::Graphic::LinearFrameBuffer(windowBuffer, response.getWidth(), response.getHeight(),
        response.getColorDepth(), response.getWidth() * bytesPerPixel, false);

    if (!client.sendSignal(CLIENT_WINDOW_INITIALIZED)) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Window: Pipe closed!");
    }
}

Window::~Window() {
    delete sharedMemory;
    delete lfb;
}

Util::Graphic::LinearFrameBuffer& Window::getFrameBuffer() const {
    return *lfb;
}

bool Window::flush() const {
    if (!client.sendRequest(Request::Flush(id))) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Window: Pipe closed!");
    }

    auto response = Response::Flush();
    if (!client.receiveResponse(response)) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Window: Pipe closed!");
    }

    return response.isSuccess();
}

bool Window::setTitle(const Util::String &title) const {
    if (!client.sendRequest(Request::SetWindowTitle(id, title))) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Window: Pipe closed!");
    }

    auto response = Response::SetWindowTitle();
    if (!client.receiveResponse(response)) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Window: Pipe closed!");
    }

    return response.isSuccess();
}

bool Window::close() const {
    if (!client.sendRequest(Request::CloseWindow(id))) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Window: Pipe closed!");
    }

    auto response = Response::CloseWindow();
    if (!client.receiveResponse(response)) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Window: Pipe closed!");
    }

    return response.isSuccess();
}

}
