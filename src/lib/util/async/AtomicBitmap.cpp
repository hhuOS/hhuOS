/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "AtomicBitmap.h"

#include "util/async/Atomic.h"
#include "util/base/Address.h"
#include "util/base/Panic.h"

namespace Util {
namespace Async {

AtomicBitmap::AtomicBitmap(const size_t blockCount) : blocks(blockCount) {
    arraySize = blockCount % SIZE_BITS == 0 ? blockCount / SIZE_BITS : blockCount / SIZE_BITS + 1;
    bitmap = new size_t[arraySize];
    Address(bitmap).setRange(0, arraySize * sizeof(size_t));
}

AtomicBitmap::~AtomicBitmap() {
    delete[] bitmap;
}

void AtomicBitmap::set(const size_t block) const {
    if (block >= blocks) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "AtomicBitmap: Block index out of bounds!");
    }

    const size_t index = block / SIZE_BITS;
    const size_t bit = block % SIZE_BITS;

    Atomic<size_t> bitmapWrapper(bitmap[index]);
    bitmapWrapper.bitSet(SIZE_BITS - 1 - bit);
}

void AtomicBitmap::unset(const size_t block) const {
    if (block >= blocks) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "AtomicBitmap: Block index out of bounds!");
    }

    const size_t index = block / SIZE_BITS;
    const size_t bit = block % SIZE_BITS;

    Atomic<size_t> bitmapWrapper(bitmap[index]);
    bitmapWrapper.bitUnset(SIZE_BITS - 1 - bit);
}

bool AtomicBitmap::check(const size_t block) const {
    if (block >= blocks) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "AtomicBitmap: Block index out of bounds!");
    }

    const size_t index = block / SIZE_BITS;
    const size_t bit = block % SIZE_BITS;

    Atomic<size_t> bitmapWrapper(bitmap[index]);
    return bitmapWrapper.bitTest(SIZE_BITS - 1 - bit);
}

size_t AtomicBitmap::findAndSet() const {
    size_t i;

    for (i = 0; i < blocks; i++) {
        const size_t index = i / SIZE_BITS;
        const size_t bit = i % SIZE_BITS;

        Atomic<size_t> bitmapWrapper(bitmap[index]);
        if (!bitmapWrapper.bitTestAndSet(SIZE_BITS - 1 - bit)) {
            break;
        }
    }

    return i == blocks ? INVALID_INDEX : i;
}

size_t AtomicBitmap::findAndUnset() const {
    size_t i;

    for (i = 0; i < blocks; i++) {
        const size_t index = i / SIZE_BITS;
        const size_t bit = i % SIZE_BITS;

        Atomic<size_t> bitmapWrapper(bitmap[index]);
        if (bitmapWrapper.bitTestAndUnset(SIZE_BITS - 1 - bit)) {
            break;
        }
    }

    return i == blocks ? INVALID_INDEX : i;
}

}
}
