#include "Bitmap.h"

namespace Util {

extern "C" {
    #include "lib/libc/string.h"
}

Bitmap::Bitmap(uint32_t blockCount) : blocks(blockCount) {
    arraySize = (blockCount % 32 == 0) ? (blockCount / 32) : (blockCount / 32 + 1);

    bitmap = new uint32_t[arraySize];

    memset(bitmap, 0, arraySize * sizeof(uint32_t));
}

Bitmap::Bitmap(uint32_t blockCount, uint32_t reservedBlocksAtBeginning) {
    arraySize = (blockCount % 32 == 0) ? (blockCount / 32) : (blockCount / 32 + 1);

    bitmap = new uint32_t[arraySize];

    memset(bitmap, 0, arraySize * sizeof(uint32_t));

    setRange(0, reservedBlocksAtBeginning);
}

Bitmap::~Bitmap() {
    delete bitmap;
}

uint32_t Bitmap::getSize() {
    return blocks;
}

void Bitmap::set(uint32_t block) {
    if(block >= blocks) {
        return;
    }

    uint32_t index = block / 32;
    uint32_t bit = block % 32;

    bitmap[index] |= 1 << (31 - bit);
}

void Bitmap::unset(uint32_t block) {
    if(block >= blocks) {
        return;
    }

    uint32_t index = block / 32;
    uint32_t bit = block % 32;

    bitmap[index] &= ~(1 << (31 - bit));
}

bool Bitmap::check(uint32_t block, bool set) {
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

    //return !(((bitmap[index] & 1 << (31 - bit)) == 1) ^ set);
}

void Bitmap::setRange(uint32_t startBlock, uint32_t length) {
    for(uint32_t i = 0; i < length; i++) {
        if(startBlock + i >= blocks) {
            return;
        }

        set(startBlock + i);
    }
}

void Bitmap::unsetRange(uint32_t startBlock, uint32_t length) {
    for(uint32_t i = 0; i < length; i++) {
        if(startBlock + i >= blocks) {
            return;
        }

        unset(startBlock + i);
    }
}

bool Bitmap::checkRange(uint32_t startBlock, uint32_t length, bool set) {
    for(uint32_t i = 0; i < length; i++) {
        if(startBlock + i >= blocks) {
            return true;
        }

        if(!check(startBlock + i, set)) {
            return false;
        }
    }

    return true;
}

uint32_t Bitmap::findAndSet(uint32_t blockCount) {
    uint32_t currentBlock = 0;

    while(currentBlock < blocks) {
        uint32_t nextSet = findNext(currentBlock, true);

        if(nextSet - currentBlock >= blockCount) {
            setRange(currentBlock, blockCount);

            return currentBlock;
        }

        currentBlock = findNext(nextSet, false);
    }

    return blocks;
}

uint32_t Bitmap::findNext(uint32_t startBlock, bool set) {
    for(uint32_t i = startBlock; i < blocks; i++) {
        if(check(i, set)) {
            return i;
        }
    }

    return blocks;
}

}