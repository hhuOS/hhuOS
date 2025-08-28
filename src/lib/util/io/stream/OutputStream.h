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

#ifndef HHUOS_LIB_UTIL_IO_OUTPUTSTREAM_H
#define HHUOS_LIB_UTIL_IO_OUTPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

namespace Util::Io {

/// Base class for output streams. An output stream is a destination for writing data.
/// The actual destination can be a file, memory or any other data sink, depending on the subclass.
/// Data is treated as a stream of raw bytes by the interface write methods.
class OutputStream {

public:
    /// No state needs to be initialized in the base class, so the default constructor is sufficient.
    OutputStream() = default;

    /// Streams are not copyable, as subclasses might manage resources that cannot be copied.
    OutputStream(const OutputStream &copy) = delete;

    /// Streams are not copyable, as subclasses might manage resources that cannot be copied.
    OutputStream &operator=(const OutputStream &copy) = delete;

    /// The output stream base class has no state, so the default destructor is sufficient.
    virtual ~OutputStream() = default;

    /// Write a single byte to the stream.
    /// On success, true is returned.
    /// If an error occurs, false is returned.
    virtual bool write(uint8_t byte) = 0;

    /// Write up to length bytes from the sourceBuffer, starting at the given offset, to the stream.
    /// The number of bytes actually written is returned,
    /// which may be less than length if the stream cannot accept more data.
    virtual size_t write(const uint8_t *sourceBuffer, size_t offset, size_t length) = 0;

    /// If the stream uses buffering, this method should flush any buffered data to the underlying destination.
    /// The number of bytes flushed is returned.
    /// The default implementation does nothing and returns 0.
    virtual size_t flush();
};

}

#endif
