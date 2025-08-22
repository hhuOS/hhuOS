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

#ifndef HHUOS_BYTEARRAYOUTPUTSTREAM_H
#define HHUOS_BYTEARRAYOUTPUTSTREAM_H

#include <stdint.h>

#include "lib/util/base/String.h"
#include "OutputStream.h"

namespace Util::Io {

class ByteArrayOutputStream : public OutputStream {

public:

    ByteArrayOutputStream();

    explicit ByteArrayOutputStream(uint32_t size);
	
	ByteArrayOutputStream(uint8_t * buffer, uint32_t size);

    ByteArrayOutputStream(const ByteArrayOutputStream &copy) = delete;

    ByteArrayOutputStream &operator=(const ByteArrayOutputStream &copy) = delete;

    ~ByteArrayOutputStream() override;

    [[nodiscard]] uint32_t getLength() const;

    [[nodiscard]] bool isEmpty() const;

    [[nodiscard]] uint32_t getPosition() const;

    [[nodiscard]] uint8_t* getBuffer() const;
	
	bool sizeLimitReached();
	
	void setSizeCheck(bool value);

    void getContent(uint8_t *target, uint32_t length) const;

    [[nodiscard]] String getContent() const;

    void reset();

    bool write(uint8_t c) override;

    uint32_t write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

private:

    uint32_t ensureRemainingCapacity(uint32_t); //returns number of bytes allowed to be written

    uint8_t *buffer;
    uint32_t size;
    uint32_t position = 0;
	
	bool allocatedBuffer = true; //has this object allocated its own buffer?
	bool checkSize = false; //should writes stop at size limit?

    static const constexpr uint32_t DEFAULT_BUFFER_SIZE = 32;
};

}

#endif
