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

#include "WindowManagerPipe.h"

#include "lib/interface.h"
#include "kepler/Protocol.h"
#include "util/async/Process.h"

namespace Kepler {

WindowManagerPipe::WindowManagerPipe() {
    const auto desktopFile = Util::Io::File("/system/kepler");
    while (!desktopFile.exists()) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofSeconds(1));
    }

    auto desktopFileStream = Util::Io::FileInputStream(desktopFile);
    const auto line = desktopFileStream.readLine();
    windowManagerProcessId = Util::String::parseNumber<size_t>(line.content);
    const auto processId = Util::Async::Process::getCurrentProcess().getId();

    createPipe("window-manager");
    const auto pipePath = Util::String::format("/process/%u/pipes/window-manager", processId);
    inputStream = new Util::Io::FileInputStream(pipePath);

    const auto pipeDirectoryPath = Util::String::format("/process/%u/pipes", windowManagerProcessId);
    const auto pipeDirectory = Util::Io::File(pipeDirectoryPath);
    while (true) {
        size_t maxId = 0;
        for (const auto &child : pipeDirectory.getChildren()) {
            const auto currentId = Util::String::parseNumber<size_t>(child.getName());
            if (currentId > maxId) {
                maxId = currentId;
            }
        }

        const auto outputFile = Util::Io::File(Util::String::format("/process/%u/pipes/%u", windowManagerProcessId, maxId));
        if (!outputFile.exists()) {
            Util::Async::Thread::yield();
            continue;
        }

        outputStream = new Util::Io::FileOutputStream(outputFile);

        const auto request = Request::Connect(processId, "window-manager");
        if (request.writeToStream(*outputStream)) {
            break;
        }
    }
}

WindowManagerPipe::~WindowManagerPipe() {
    delete inputStream;
    delete outputStream;
}

bool WindowManagerPipe::sendRequest(const Util::Async::Streamable &streamable) const {
    return streamable.writeToStream(*outputStream);
}

bool WindowManagerPipe::receiveResponse(Util::Async::Streamable &streamable) const {
    return streamable.readFromStream(*inputStream);
}

size_t WindowManagerPipe::getWindowManagerProcessId() const {
    return windowManagerProcessId;
}

}
