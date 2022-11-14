/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#ifndef HHUOS_UTIL_H
#define HHUOS_UTIL_H

#include "lib/util/stream/InputStream.h"

namespace Network {

class NumberUtil {

public:
    /**
     * Default Constructor.
     */
    NumberUtil() = default;

    /**
     * Copy Constructor.
     */
    NumberUtil(const NumberUtil &other) = delete;

    /**
     * Assignment operator.
     */
    NumberUtil &operator=(const NumberUtil &other) = delete;

    /**
     * Destructor.
     */
    ~NumberUtil() = default;

    static int16_t read16BitValue(Util::Stream::InputStream &stream);

    static uint16_t readUnsigned16BitValue(Util::Stream::InputStream &stream);

    static int32_t read32BitValue(Util::Stream::InputStream &stream);

    static uint32_t readUnsigned32BitValue(Util::Stream::InputStream &stream);
};

}

#endif
