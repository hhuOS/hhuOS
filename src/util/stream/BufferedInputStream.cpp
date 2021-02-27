#include <util/memory/Address.h>
#include "BufferedInputStream.h"

namespace Util::Stream {

BufferedInputStream::BufferedInputStream(InputStream &stream) : BufferedInputStream(stream, DEFAULT_BUFFER_SIZE) {}

BufferedInputStream::BufferedInputStream(InputStream &stream, uint32_t size) : FilterInputStream(stream), buffer(new uint8_t[size]), size(size) {}

BufferedInputStream::~BufferedInputStream() {
    delete[] buffer;
}

int16_t BufferedInputStream::read() {
    if (position >= valid && !refill()) {
        return -1;
    }

    return buffer[position++];
}

uint32_t BufferedInputStream::read(uint8_t *target, uint32_t offset, uint32_t length) {
    if (length == 0) {
        return 0;
    }

    if (position >= valid && !refill()) {
        return -1;
    }

    uint32_t ret = 0;

    do {
        uint32_t readCount = valid - position > length ? length : valid - position;
        Memory::Address<uint32_t> sourceAddress(buffer + position);
        Memory::Address<uint32_t> targetAddress(target + offset);
        targetAddress.copyRange(sourceAddress, readCount);

        position += readCount;
        offset += readCount;
        length -= readCount;
        ret += readCount;
    } while (length > 0 && refill());

    return ret;
}

void BufferedInputStream::close() {
    FilterInputStream::close();
}

bool BufferedInputStream::refill() {
    int readCount = FilterInputStream::read(buffer, valid, size - valid);

    if (readCount <= 0) {
        return false;
    }

    valid += readCount;
    return true;
}

}