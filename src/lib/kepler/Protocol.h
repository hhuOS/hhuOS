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

#ifndef HHUOS_WINDOW_MANAGER_PROTOCOL_H
#define HHUOS_WINDOW_MANAGER_PROTOCOL_H

#include <stddef.h>

#include "util/async/Streamable.h"
#include "util/io/stream/InputStream.h"

namespace Kepler {

enum Command : uint8_t {
    CONNECT,
    CREATE_WINDOW,
    FLUSH
};

namespace Request {

class Connect final : public Util::Async::Streamable {

public:

    Connect() = default;

    Connect(size_t processId, const Util::String &pipeName);

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    [[nodiscard]] Util::String getPipePath() const;

private:

    size_t processId = 0;
    Util::String pipeName;
};

class CreateWindow final : public Util::Async::Streamable {

public:

    CreateWindow() = default;

    CreateWindow(uint16_t width, uint16_t height, const Util::String &title);

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    [[nodiscard]] uint16_t getWidth() const;

    [[nodiscard]] uint16_t getHeight() const;

    [[nodiscard]] Util::String getTitle() const;

private:

    uint16_t width = 0;
    uint16_t height = 0;
    Util::String title;
};

class Flush final : public Util::Async::Streamable {

public:

    Flush() = default;

    explicit Flush(size_t windowId);

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    [[nodiscard]] size_t getWindowId() const;

private:

    size_t windowId = 0;
};

}

namespace Response {

class CreateWindow final : public Util::Async::Streamable {

public:

    CreateWindow() = default;

    CreateWindow(size_t id, uint16_t width, uint16_t height, uint8_t colorDepth);

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    [[nodiscard]] size_t getId() const;

    [[nodiscard]] uint16_t getSizeX() const;

    [[nodiscard]] uint16_t getSizeY() const;

    [[nodiscard]] uint8_t getColorDepth() const;

private:

    size_t id = 0;
    uint16_t width = 0;
    uint16_t height = 0;
    uint8_t colorDepth = 0;
};

class Flush final : public Util::Async::Streamable {

public:

    Flush() = default;

    explicit Flush(bool success);

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    [[nodiscard]] bool isSuccess() const;

private:

    bool success = false;

};

}

}

#endif