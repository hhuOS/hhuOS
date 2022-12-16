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

#ifndef HHUOS_FILTERINPUTSTREAM_H
#define HHUOS_FILTERINPUTSTREAM_H

#include <cstdint>

#include "InputStream.h"

namespace Util::Stream {

class FilterInputStream : public InputStream {

public:

    explicit FilterInputStream(InputStream &stream);

    FilterInputStream(const FilterInputStream &copy) = delete;

    FilterInputStream &operator=(const FilterInputStream &copy) = delete;

    ~FilterInputStream() override = default;

    int16_t read() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

private:

    InputStream &stream;
};

}

#endif
