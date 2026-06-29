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

#ifndef HHUOS_LIB_KEPLER_CLIENT_H
#define HHUOS_LIB_KEPLER_CLIENT_H

#include "EventListener.h"
#include "EventRunnable.h"

#include <util/async/Streamable.h>
#include <util/io/stream/FileInputStream.h>
#include <util/io/stream/FileOutputStream.h>

namespace Kepler {

class Client {

public:

    Client() = default;

    Client(const Client &other) = delete;

    Client& operator=(const Client &other) = delete;

    ~Client();

    bool connect();

    bool isConnected() const {
        return windowManagerProcessId != 0;
    }

    bool sendRequest(const Util::Async::Streamable &streamable) const;

    bool receiveResponse(Util::Async::Streamable &streamable) const;

    size_t getWindowManagerProcessId() const {
        return windowManagerProcessId;
    }

    void registerEventListener(size_t windowId, EventListener &listener) const;

    static Client& getInstance();

private:

    size_t windowManagerProcessId = 0;

    Util::Io::FileOutputStream *requestOutputStream = nullptr;
    Util::Io::FileInputStream *responseInputStream = nullptr;

    EventRunnable *eventRunnable = nullptr;
};

}

#endif