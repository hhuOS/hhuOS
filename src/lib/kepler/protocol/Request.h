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

#ifndef HHUOS_REQUEST_H
#define HHUOS_REQUEST_H

#include <stddef.h>
#include <stdint.h>

#include <util/async/Streamable.h>
#include <util/base/String.h>

namespace Kepler {
namespace Request {

enum Command : uint8_t {
    CONNECT,
    CREATE_WINDOW,
    SET_WINDOW_TITLE,
    FLUSH,
    CLOSE_WINDOW
};

class Connect final : public Util::Async::Streamable {

public:

    Connect() = default;

    Connect(const size_t processId, const Util::String &pipeName) : processId(processId), pipeName(pipeName) {}

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    Util::String getPipePath() const {
        return Util::String::format("/process/%u/pipes/%s", processId, static_cast<const char*>(pipeName));
    }

    size_t getProcessId() const {
        return processId;
    }

private:

    size_t processId = 0;
    Util::String pipeName;
};

class CreateWindow final : public Util::Async::Streamable {

public:

    CreateWindow() = default;

    CreateWindow(const uint16_t width, const uint16_t height, const Util::String &title) :
        width(width), height(height), title(title) {}

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    uint16_t getWidth() const {
        return width;
    }

    uint16_t getHeight() const {
        return height;
    }

    const Util::String& getTitle() const {
        return title;
    }

private:

    uint16_t width = 0;
    uint16_t height = 0;
    Util::String title;
};

class BasicWindowRequest : public Util::Async::Streamable {

public:
    
    explicit BasicWindowRequest(const Command command) : command(command) {}

    BasicWindowRequest(const Command command, const size_t windowId) : command(command), windowId(windowId) {}

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    size_t getWindowId() const {
        return windowId;
    }

private:

    Command command;
    size_t windowId = 0;
};

class SetWindowTitle final : public BasicWindowRequest {

public:

    SetWindowTitle() : BasicWindowRequest(SET_WINDOW_TITLE) {}

    explicit SetWindowTitle(const size_t windowId, const Util::String &title) :
        BasicWindowRequest(SET_WINDOW_TITLE, windowId), title(title) {}

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    const Util::String& getTitle() const {
        return title;
    }

private:

    Util::String title;
};

class CloseWindow final : public BasicWindowRequest {

public:

    CloseWindow() : BasicWindowRequest(CLOSE_WINDOW) {}

    explicit CloseWindow(const size_t windowId) : BasicWindowRequest(CLOSE_WINDOW, windowId) {}
};

class Flush final : public BasicWindowRequest {

public:

    Flush() : BasicWindowRequest(FLUSH) {}

    explicit Flush(const size_t windowId) : BasicWindowRequest(FLUSH, windowId) {}
};

}
}

#endif
