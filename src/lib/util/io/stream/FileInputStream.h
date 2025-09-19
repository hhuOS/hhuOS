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

#ifndef HHUOS_LIB_UTIL_IO_FILEINPUTSTREAM_H
#define HHUOS_LIB_UTIL_IO_FILEINPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "base/String.h"
#include "io/file/File.h"
#include "io/stream/InputStream.h"

namespace Util::Io {

/// An input stream that reads data from a file.
/// The file is specified by its path or a `File` instance.
/// The stream maintains a current position within the file, which is advanced as data is read.
/// The current position in the file can be queried and modified.
/// The access mode of the underlying file can be switched between blocking and non-blocking.
/// In blocking mode, read operations will wait until data is available.
/// In non-blocking mode, read operations will return immediately if no data is available.
/// By default, the stream operates in blocking mode.
/// This stream does not perform any buffering; each read operation leads to a system call.
/// For performance reasons, it is recommended to wrap this stream in a `BufferedInputStream`.
///
/// ## Example
/// ```c++
/// // Create a file input stream to read from a file
/// const auto file = Util::File("/path/to/file.txt");
/// Util::Io::FileInputStream fileStream(file);
///
/// // Read a single byte from the file
/// const auto byte = fileStream.read();
///
///	// Read a line from the file
/// fileStream.setPosition(0); // Reset position to the beginning of the file
/// const auto line = fileStream.readLine();
///
/// // Read the entire file content into a buffer
/// uint8_t *buffer = new uint8_t[file.getSize()];
/// fileStream.setPosition(0); // Reset position to the beginning of the file
/// fileStream.read(buffer, 0, file.getSize());
/// ```
class FileInputStream final : public InputStream {

public:
	/// Create a file input stream instance that reads from the file at the given path.
	/// If the file does not exist or cannot be opened, a panic is fired.
    explicit FileInputStream(const String &path);

	/// Create a file input stream instance that reads from the given file.
	/// If the file does not exist or cannot be opened, a panic is fired.
    explicit FileInputStream(const File &file);

	/// Create a file input stream instance that reads from the file associated with the given file descriptor.
	/// The instance does not take ownership of the file descriptor and will not close it upon destruction.
	/// It is the caller's responsibility to ensure that the file descriptor remains valid
	/// for the lifetime of this input stream and to close it when no longer needed.
    explicit FileInputStream(int32_t fileDescriptor);

	/// Destroy the file input stream instance and close the file descriptor if it was opened by this instance.
    ~FileInputStream() override;

	/// Read a single byte from the file.
	/// On success, the byte is returned as an integer in the range 0 to 255.
	/// If an error occurs or the end of the file is reached, -1 is returned.
	/// If no byte is currently available, but the end of the file has not been reached (e.g. for pipes or sockets),
	/// the behavior depends on the current access mode:
	/// In blocking mode, the call will block until data is available.
	/// In non-blocking mode, -1 is returned immediately. In this case, the stream is not at the end of the file,
	/// but there is currently no data available to read.
    int16_t read() override;

	/// Peek at the next byte in the file without removing it from the stream.
	/// If an error occurs or the end of the file is reached, -1 is returned.
	/// If no byte is currently available, but the end of the file has not been reached (e.g. for pipes or sockets),
	/// the behavior depends on the current access mode:
	/// In blocking mode, the call will block until data is available.
	/// In non-blocking mode, -1 is returned immediately. In this case, the stream is not at the end of the file,
	/// but there is currently no data available to read.
	int16_t peek() override;

	/// Read up to length bytes from the file into the target buffer, starting at the given offset.
	/// On success, the number of bytes read is returned (which may be less than length if fewer bytes are available).
	/// If an error occurs or the end of the file is reached before reading any bytes, -1 is returned.
	/// It is the caller's responsibility to ensure that the target buffer has enough space for offset + length bytes.
	/// If the buffer is too small, data is written out of bounds, leading to undefined behavior.
	/// If no bytes are currently available, but the end of the file has not been reached (e.g. for pipes or sockets),
	/// the behavior depends on the current access mode:
	/// In blocking mode, the call will block until data is available.
	/// In non-blocking mode, 0 is returned immediately. In this case, the stream is not at the end of the file,
	/// but there is currently no data available to read.
    int32_t read(uint8_t *targetBuffer, size_t offset, size_t length) override;

	/// Check if there is data available to read from the file.
	/// If this method returns true, a subsequent read call is guaranteed to succeed without blocking.
	bool isReadyToRead() override;

	/// Move the current position in the file to the given offset, based on the specified seek mode.
	/// The seek mode can be one of the following:
	/// - `File::SeekMode::SET`: Set the position to the specified offset from the beginning of the file.
	/// - `File::SeekMode::CURRENT`: Set the position to the current position plus the specified offset.
	/// - `File::SeekMode::END`: Set the position to the end of the file plus the specified offset.
	void setPosition(int64_t offset, File::SeekMode mode = File::SeekMode::SET);

	/// Get the current position in the file, i.e., the offset in bytes from the beginning of the file.
	[[nodiscard]] size_t getPosition() const;

	/// Set the access mode of the underlying file.
	/// In blocking mode, read operations will wait until data is available.
	/// In non-blocking mode, read operations will return immediately if no data is available.
	/// By default, the stream operates in blocking mode.
	/// On success, true is returned. If an error occurs, false is returned.
	bool setAccessMode(File::AccessMode mode);

private:

	bool closeFileDescriptor = true;
	int32_t fileDescriptor = -1;
	uint64_t pos = 0;

	int16_t peekedChar = -1;
};

}

#endif
