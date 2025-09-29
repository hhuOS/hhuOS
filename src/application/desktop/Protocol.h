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

#include "io/stream/InputStream.h"

enum Command : uint8_t {
    FLUSH
};

struct CreateWindowRequest {
    uint16_t resX;
    uint16_t resY;

    size_t processId;
    Util::String pipeName;

    bool writeToStream(Util::Io::OutputStream &stream) const;

    [[nodiscard]] Util::String getPipePath() const;

    static CreateWindowRequest readFromStream(Util::Io::InputStream &stream);
};

struct CreateWindowResponse {
    uint16_t resX;
    uint16_t resY;
    uint8_t colorDepth;
    size_t sharedBufferId;
    size_t sharedBufferPageCount;

    void writeToStream(Util::Io::OutputStream &stream) const;

    static CreateWindowResponse readFromStream(Util::Io::InputStream &stream);
};

#endif