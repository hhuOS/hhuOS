/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_BUFFEREDWRITER_H
#define HHUOS_BUFFEREDWRITER_H

#include <cstdint>

#include "Writer.h"
#include "lib/util/memory/String.h"

namespace Util::Stream {

class BufferedWriter : public Writer {

public:

    explicit BufferedWriter(Writer &writer, uint32_t size = DEFAULT_BUFFER_SIZE);

    BufferedWriter(const BufferedWriter &copy) = delete;

    BufferedWriter &operator=(const BufferedWriter &copy) = delete;

    ~BufferedWriter() override;

    void flush() override;

    void write(char c) override;

    void write(const char *sourceBuffer, uint32_t length) override;

    void write(const char *sourceBuffer, uint32_t offset, uint32_t length) override;

    void write(const Util::Memory::String &string) override;

    void write(const Util::Memory::String &string, uint32_t offset, uint32_t length) override;

private:

    Writer &writer;
    char *buffer;
    uint32_t size;
    uint32_t position = 0;

    static const constexpr uint32_t DEFAULT_BUFFER_SIZE = 8192;

};

}

#endif
