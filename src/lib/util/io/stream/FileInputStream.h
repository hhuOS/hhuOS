/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_FILEINPUTSTREAM_H
#define HHUOS_FILEINPUTSTREAM_H

#include <stdint.h>

#include "InputStream.h"
#include "FileStream.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Util::Io {

/**
 * A stream that reads data from a file.
 * It is essentially a wrapper for FileStream, that opens the file in read mode.
 */
class FileInputStream : public InputStream {

public:

    explicit FileInputStream(const Io::File &file);

    explicit FileInputStream(const String &path);

    explicit FileInputStream(int32_t fileDescriptor);

    FileInputStream(const FileInputStream &copy) = delete;

    FileInputStream &operator=(const FileInputStream &copy) = delete;

    int16_t read() override;
	
	int16_t peek() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

    bool setAccessMode(File::AccessMode accessMode) const;

    bool isReadyToRead() override;

private:
	FileStream fileStream;

};

}

#endif
