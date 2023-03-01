/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/base/Address.h"
#include "lib/util/async/Atomic.h"
#include "AtomicBitmap.h"

namespace Util::Async {

AtomicBitmap::AtomicBitmap(uint32_t blockCount) : blocks(blockCount) {
    arraySize = (blockCount % 32 == 0) ? (blockCount / 32) : (blockCount / 32 + 1);
    bitmap = new uint32_t[arraySize];
    Address<uint32_t>(bitmap).setRange(0, arraySize * sizeof(uint32_t));
}

uint32_t AtomicBitmap::getSize() const {
    return blocks;
}

void AtomicBitmap::set(uint32_t block) {
    if (block >= blocks) {
        return;
    }

    uint32_t index = block / 32;
    uint32_t bit = block % 32;

    Async::Atomic<uint32_t> bitmapWrapper(bitmap[index]);
    bitmapWrapper.bitSet(31 - bit);
}

void AtomicBitmap::unset(uint32_t block) {
    if (block >= blocks) {
        return;
    }

    uint32_t index = block / 32;
    uint32_t bit = block % 32;

    Async::Atomic<uint32_t> bitmapWrapper(bitmap[index]);
    bitmapWrapper.bitReset(31 - bit);
}

bool AtomicBitmap::check(uint32_t block, bool set) {
    if (block >= blocks) {
        return false;
    }

    uint32_t index = block / 32;
    uint32_t bit = block % 32;

    Async::Atomic<uint32_t> bitmapWrapper(bitmap[index]);
    return bitmapWrapper.bitTest(31 - bit) == set;
}

uint32_t AtomicBitmap::findAndSet() {
    uint32_t i;

    for (i = 0; i < blocks; i++) {
        uint32_t index = i / 32;
        uint32_t bit = i % 32;

        Async::Atomic<uint32_t> bitmapWrapper(bitmap[index]);
        if (!bitmapWrapper.bitTestAndSet(31 - bit)) {
            break;
        }
    }

    if (i == blocks) {
        return INVALID_INDEX;
    }

    return i;
}

uint32_t AtomicBitmap::findAndUnset() {
    uint32_t i;

    for (i = 0; i < blocks; i++) {
        uint32_t index = i / 32;
        uint32_t bit = i % 32;

        Async::Atomic<uint32_t> bitmapWrapper(bitmap[index]);
        if (bitmapWrapper.bitTestAndReset(31 - bit)) {
            break;
        }
    }

    if (i == blocks) {
        return INVALID_INDEX;
    }

    return i;
}

}