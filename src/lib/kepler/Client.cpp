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

#include "Client.h"

#include "protocol/Request.h"

#include <interface.h>
#include <util/async/Process.h>
#include <util/io/stream/NumberUtil.h>

namespace Kepler {

Client::~Client() {
    destroyPipe("kepler-event");
    delete responseInputStream;
    delete requestOutputStream;
}

bool Client::connect() {
    const Util::Io::File desktopFile("/system/kepler");
    while (!desktopFile.exists()) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofSeconds(1));
    }

    Util::Io::FileInputStream desktopFileStream(desktopFile);
    const auto line = desktopFileStream.readLine();
    windowManagerProcessId = Util::String::parseNumber<size_t>(line.content);
    const auto processId = Util::Async::Process::getCurrentProcess().getId();

    createPipe("kepler");
    const auto pipePath = Util::String::format("/process/%u/pipes/kepler", processId);
    responseInputStream = new Util::Io::FileInputStream(pipePath);

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

        requestOutputStream = new Util::Io::FileOutputStream(outputFile);

        const auto request = Request::Connect(processId, "kepler");
        if (request.writeToStream(*requestOutputStream)) {
            break;
        }
    }

    createPipe("kepler-event");
    eventRunnable = new EventRunnable(Util::String::format("/process/%u/pipes/kepler-event", processId));
    Util::Async::Thread::createThread("Kepler-Event", eventRunnable);

    return true;
}

bool Client::sendRequest(const Util::Async::Streamable &streamable) const {
    if (requestOutputStream == nullptr) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Client: Not connected to a kepler instance yet!");
    }

    return streamable.writeToStream(*requestOutputStream);
}

bool Client::receiveResponse(Util::Async::Streamable &streamable) const {
    if (responseInputStream == nullptr) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Client: Not connected to a kepler instance yet!");
    }

    return streamable.readFromStream(*responseInputStream);
}

void Client::registerEventListener(const size_t windowId, EventListener &listener) const {
    if (eventRunnable == nullptr) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Client: Not connected to a kepler instance yet!");
    }

    eventRunnable->registerListener(windowId, listener);
}

Client& Client::getInstance() {
    static Client client;
    if (!client.isConnected()) {
        client.connect();
    }

    return client;
}

}
