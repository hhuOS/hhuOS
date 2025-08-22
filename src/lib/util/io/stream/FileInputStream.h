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

#ifndef HHUOS_FILEINPUTSTREAM_H
#define HHUOS_FILEINPUTSTREAM_H

#include <stdint.h>

#include "InputStream.h"
#include "async/Spinlock.h"
#include "collection/ArrayQueue.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Util::Io {

class FileInputStream : public InputStream {

public:

    explicit FileInputStream(const File &file);

    explicit FileInputStream(const String &path);

    explicit FileInputStream(int32_t fileDescriptor);

    FileInputStream(const FileInputStream &copy) = delete;

    FileInputStream &operator=(const FileInputStream &copy) = delete;

    int16_t read() override;
	
	int16_t peek() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

	bool isReadyToRead() override;

	void setPosition(uint32_t offset, File::SeekMode mode = File::SeekMode::SET);

	[[nodiscard]] uint32_t getPosition() const;

	bool pushBack(uint8_t c);

	bool setAccessMode(File::AccessMode mode);

private:

	int32_t fileDescriptor = -1;
	uint32_t pos = 0;

	int16_t pushedBackByte = -1;
};

}

#endif
