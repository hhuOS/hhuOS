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

#ifndef HHUOS_LIB_UTIL_IO_FILEOUTPUTSTREAM_H
#define HHUOS_LIB_UTIL_IO_FILEOUTPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "util/base/String.h"
#include "util/io/file/File.h"
#include "util/io/stream/OutputStream.h"

namespace Util {
namespace Io {

/// An output stream that writes data to a file.
/// The file is specified by its path, or a `File` instance.
/// The stream maintains a current position within the file, which is advanced as data is written.
/// The current position in the file can be queried and modified.
/// This stream does not perform any buffering; each write operation leads to a system call.
/// For performance reasons, it is recommended to wrap this stream in a `BufferedOutputStream`.
///
/// ## Example
/// ```c++
/// const auto file = Util::File("/path/to/file.txt");
/// if (!file.exists()) {
///     file.create();
/// }
///
/// FileOutputStream fileStream(file);
///
/// // Write data to the file one byte at a time
/// fileStream.write('H');
/// fileStream.write('e');
/// fileStream.write('l');
/// fileStream.write('l');
/// fileStream.write('o');
/// fileStream.write(',');
/// fileStream.write(' ');
///
/// // Write a byte array to the file in one operation
/// const char *world = "World!";
/// fileStream.write(reinterpret_cast<const uint8_t*>(world), 0, 6);
/// ```
class FileOutputStream final : public OutputStream {

public:
    /// Create a file output stream instance that writes to the file at the given path.
    /// If the file does not exist, a panic is fired.
    explicit FileOutputStream(const String &path);

    /// Create a file output stream instance that writes to the given file.
    /// If the file does not exist, a panic is fired.
    explicit FileOutputStream(const File &file) : FileOutputStream(file.getCanonicalPath()) {}

    /// Create a file output stream instance that writes to the file associated with the given file descriptor.
    /// The instance does not take ownership of the file descriptor and will not close it upon destruction.
    /// It is the caller's responsibility to ensure that the file descriptor remains valid
    /// for the lifetime of this output stream to close it when no longer needed.
    explicit FileOutputStream(const int32_t fileDescriptor) :
        closeFileDescriptor(false), fileDescriptor(fileDescriptor) {}

    /// Destroy the file output stream and close the file descriptor if it was opened by this instance.
    ~FileOutputStream() override;

    /// Write a single byte to the file.
    /// On success, true is returned. On failure, false is returned.
    /// If the file is a regular file and the current position is at the end of the file,
    /// the file is extended by one byte.
    bool write(uint8_t byte) override;

    /// Write up to length bytes from the sourceBuffer, starting at the given offset, to the file.
    /// The number of bytes actually written is returned, which may be less than length if an error occurs.
    /// If the file is a regular file and the current position is at the end of the file,
    /// the file is extended to accommodate the new data.
    size_t write(const uint8_t *sourceBuffer, size_t offset, size_t length) override;

    /// Move the current position in the file to the given offset, based on the specified seek mode.
    /// The seek mode can be one of the following:
    /// - `File::SeekMode::SET`: Set the position to the specified offset from the beginning of the file.
    /// - `File::SeekMode::CURRENT`: Set the position to the current position plus the specified offset.
    /// - `File::SeekMode::END`: Set the position to the end of the file plus the specified offset.
    void setPosition(size_t offset, File::SeekMode mode = File::SeekMode::SET);

    /// Get the current position in the file, i.e., the offset in bytes from the beginning of the file.
    size_t getPosition() const {
        return pos;
    }

private:

    bool closeFileDescriptor = true;
    int32_t fileDescriptor = -1;
    size_t pos = 0;
};

}
}

#endif
