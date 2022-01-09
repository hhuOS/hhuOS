#include "lib/util/async/Atomic.h"
#include "AtomicBitmap.h"
#include "Address.h"

namespace Util::Memory {

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