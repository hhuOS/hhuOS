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

#ifndef HHUOS_BYTEARRAYINPUTSTREAM_H
#define HHUOS_BYTEARRAYINPUTSTREAM_H

#include <stdint.h>

#include "InputStream.h"

namespace Util::Io {

class ByteArrayInputStream : public InputStream {

public:

    ByteArrayInputStream(uint8_t *buffer, uint32_t size);

    ByteArrayInputStream(const ByteArrayInputStream &copy) = delete;

    ByteArrayInputStream &operator=(const ByteArrayInputStream &copy) = delete;

    ~ByteArrayInputStream() override = default;

    [[nodiscard]] uint32_t getLength() const;

    [[nodiscard]] uint32_t getPosition() const;

    [[nodiscard]] uint32_t getRemaining() const;

    [[nodiscard]] bool isEmpty() const;

    [[nodiscard]] const uint8_t* getBuffer() const;
	
	void disableSizeLimit();
	
	void makeNullTerminated();

    int16_t read() override;
	
	int16_t peek() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

    bool isReadyToRead() override;

private:

    uint8_t *buffer;
    uint32_t size;
    uint32_t position = 0;
	
	bool enforceSizeLimit = true;
	bool nullTerminated = false;
};

}

#endif
