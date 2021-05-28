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

    asm volatile (
    "lock bts %1, %0"
    : "+m"(bitmap[index])
    : "r"(31 - bit)
    );
}

void AtomicBitmap::unset(uint32_t block) {
    if (block >= blocks) {
        return;
    }

    uint32_t index = block / 32;
    uint32_t bit = block % 32;

    asm volatile (
    "lock btr %1, %0"
    : "+m"(bitmap[index])
    : "r"(31 - bit)
    );
}

bool AtomicBitmap::check(uint32_t block, bool set) {
    if(block >= blocks) {
        return false;
    }

    uint32_t index = block / 32;
    uint32_t bit = block % 32;

    if(set) {
        return (bitmap[index] & (1 << (31 - bit))) != 0;
    } else {
        return (bitmap[index] & (1 << (31 - bit))) == 0;
    }
}

uint32_t AtomicBitmap::findAndSet() {
    uint32_t i;

    for (i = 0; i < blocks; i++) {
        uint32_t index = i / 32;
        uint32_t bit = i % 32;

        asm volatile (
        "lock bts %1, %0;"
        "jnc FOUND;"
        : "+m"(bitmap[index])
        : "r"(31 - bit)
        );
    }

    // NICE!
    asm volatile (
    "FOUND:"
    );

    return i;
}

}