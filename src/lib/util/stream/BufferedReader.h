#ifndef HHUOS_BUFFEREDREADER_H
#define HHUOS_BUFFEREDREADER_H

#include <cstdint>

#include "Reader.h"
#include "lib/util/memory/String.h"

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

namespace Util::Stream {

class BufferedReader : public Reader {

public:

    explicit BufferedReader(Reader &reader, uint32_t size = DEFAULT_BUFFER_SIZE);

    BufferedReader(const BufferedReader &copy) = delete;

    BufferedReader &operator=(const BufferedReader &copy) = delete;

    ~BufferedReader() override;

    char read() override;

    int32_t read(char *targetBuffer, uint32_t length) override;

    int32_t read(char *targetBuffer, uint32_t offset, uint32_t length) override;

    Memory::String read(uint32_t length) override;

    Memory::String readLine();

private:

    bool refill();

    Reader &reader;
    char *buffer;
    uint32_t size;
    uint32_t position = 0;
    uint32_t valid = 0;

    static const constexpr uint32_t DEFAULT_BUFFER_SIZE = 8192;

};

}

#endif
