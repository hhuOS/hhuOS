#include <util/memory/Address.h>
#include "ByteArrayInputStream.h"

namespace Util::Stream {

ByteArrayInputStream::ByteArrayInputStream(uint8_t *buffer, uint32_t size) : buffer(buffer), size(size) {

}

int16_t ByteArrayInputStream::read() {
    if (position >= size) {
        return -1;
    }

    return buffer[position++];
}

uint32_t ByteArrayInputStream::read(uint8_t *target, uint32_t offset, uint32_t length) {
    if (position >= size) {
        return -1;
    }

    uint32_t count = size - position > length ? length : size - position;
    auto sourceAddress = Memory::Address<uint32_t>(buffer, size).add(position);
    auto targetAddress = Memory::Address<uint32_t>(target, length + offset).add(offset);
    targetAddress.copyRange(sourceAddress, count);

    position += count;
    return count;
}

}