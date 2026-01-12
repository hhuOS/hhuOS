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

#ifndef HHUOS_LIB_UTIL_ASYNC_STREAMABLE_H
#define HHUOS_LIB_UTIL_ASYNC_STREAMABLE_H

namespace Util {
namespace Io {
class InputStream;
class OutputStream;
} // namespace Io
} // namespace Util

namespace Util {
namespace Async {

/// Interface class for objects, that can be serialized to and from streams.
class Streamable {

public:
    /// The interface does not manage any resources, so the default destructor is sufficient.
    virtual ~Streamable() = default;

    /// Write the object to the given output stream.
    /// The return value indicates whether the operation was successful.
    virtual bool writeToStream(Io::OutputStream &stream) const = 0;

    /// Read the object from the given input stream.
    /// The return value indicates whether the operation was successful.
    /// If the operation fails, the object may be in an inconsistent state.
    virtual bool readFromStream(Io::InputStream &stream) = 0;
};

}
}

#endif