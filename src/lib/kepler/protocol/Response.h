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

#ifndef HHUOS_RESPONSE_H
#define HHUOS_RESPONSE_H

#include <stddef.h>
#include <stdint.h>

#include <util/async/Streamable.h>

namespace Kepler {
namespace Response {

class BasicResponse : public Util::Async::Streamable {

public:

    BasicResponse() = default;

    explicit BasicResponse(const bool success) : success(success) {}

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    bool isSuccess() const {
        return success;
    }

private:

    bool success = false;
};

class CreateWindow final : public BasicResponse {

public:

    CreateWindow() = default;

    CreateWindow(const size_t id, const uint16_t width, const uint16_t height, const uint8_t colorDepth) :
        BasicResponse(true), id(id), width(width), height(height), colorDepth(colorDepth) {}

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    size_t getId() const {
        return id;
    }

    uint16_t getWidth() const {
        return width;
    }

    uint16_t getHeight() const {
        return height;
    }

    uint8_t getColorDepth() const {
        return colorDepth;
    }

private:

    size_t id = 0;
    uint16_t width = 0;
    uint16_t height = 0;
    uint8_t colorDepth = 0;
};

class SetWindowTitle final : public BasicResponse {

public:

    SetWindowTitle() = default;

    explicit SetWindowTitle(const bool success) : BasicResponse(success) {}
};

class Flush final : public BasicResponse {

public:

    Flush() = default;

    explicit Flush(const bool success) : BasicResponse(success) {}
};

class CloseWindow final : public BasicResponse {

public:

    CloseWindow() = default;

    explicit CloseWindow(const bool success) : BasicResponse(success) {}
};

}
}

#endif
