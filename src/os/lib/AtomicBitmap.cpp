#include <devices/cpu/Cpu.h>
#include "AtomicBitmap.h"

AtomicBitmap::AtomicBitmap(uint32_t blockCount) : Bitmap(blockCount) {

}

AtomicBitmap::AtomicBitmap(uint32_t blockCount, uint32_t reservedBlocksAtBeginning) :
        Bitmap(blockCount, reservedBlocksAtBeginning) {

}

void AtomicBitmap::set(uint32_t block) {
    if(block >= blocks) {
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
    if(block >= blocks) {
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
    return Bitmap::check(block, set);
}

uint32_t AtomicBitmap::findAndSet() {
    uint32_t i;

    for(i = 0; i < blocks; i++) {
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

uint32_t AtomicBitmap::findAndSet(uint32_t blockCount) {
    Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION, "findAndSet() is not supported by AtomicBitmap!");

    return blockCount;
}

void AtomicBitmap::setRange(uint32_t startBlock, uint32_t length) {
    Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION, "setRange() is not supported by AtomicBitmap!");
}

void AtomicBitmap::unsetRange(uint32_t startBlock, uint32_t length) {
    Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION, "unsetRange() is not supported by AtomicBitmap!");
}

bool AtomicBitmap::checkRange(uint32_t startBlock, uint32_t length, bool set) {
    Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION, "checkRange() is not supported by AtomicBitmap!");

    return false;
}
